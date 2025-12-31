import { Deferred } from "@src/utils/Deferred"
import { Mutex } from "@src/utils/Mutex"
import { DateTime } from "luxon"
import { Subject } from "rxjs"
import type { RawLogMessage } from "../models"

export class DeviceCommandExecutor {

  private activeCommands: { [index: number]: Deferred<string[]> } = {}
  private activeCommandResponses: Map<number, string[]> = new Map()
  private nextCommandId: number = 1

  private readonly sendMutex: Mutex = new Mutex()

  private readonly commandTimeoutMs = 2000
  private readonly mutexTimeoutMs = this.commandTimeoutMs + 1000

  constructor(
    private readonly writer: WritableStreamDefaultWriter,
    private readonly logsSubject: Subject<RawLogMessage>,
  ) {
  }

  async sendCommand(type: string, args: string[]): Promise<string[]> {
    const writer = this.writer
    if (!writer) {
      throw new Error("Not connected")
    }

    const commandId = this.getNextCommandId()
    const deferredCommandResult = new Deferred<string[]>()
    this.activeCommands[commandId] = deferredCommandResult

    try {
      await this.sendMutex.acquire(this.mutexTimeoutMs)

      const packet = this.formatPacket(commandId, type, args)

      console.debug(`Sending packet: '${packet.text}'`)

      await writer.write(packet.bytes)

      this.logsSubject.next({ direction: "to-device", message: packet.text, timestamp: DateTime.now() })

      const timeout = setTimeout(() => {
        deferredCommandResult.reject(new Error("command timeout"))
      }, this.commandTimeoutMs)

      const response = await deferredCommandResult.promise

      clearTimeout(timeout)

      return response
    } finally {
      delete this.activeCommands[commandId]
      this.sendMutex.release()
    }
  }

  onLineReceived(line: string) {
    if (!line.startsWith("%")) {
      return
    }

    console.debug("onLineReceived", line)

    try {
      const [responseHeader, ...rest] = line.substring(1).split(":")
      const [commandIdStr, sequenceNumberStr] = responseHeader.split(".")

      const commandId = parseInt(commandIdStr)
      const sequenceNumber = parseInt(sequenceNumberStr)
      const commandResponse = rest.join(":")

      if (!this.activeCommandResponses.has(commandId)) {
        this.activeCommandResponses.set(commandId, [])
      }

      const commandResponses = this.activeCommandResponses.get(commandId)!

      if (sequenceNumber > 0) {
        commandResponses.push(commandResponse)
      }

      if (sequenceNumber === 0) {
        this.activeCommands[commandId]?.resolve(commandResponses)
        this.activeCommandResponses.delete(commandId)
      } else if (isNaN(sequenceNumber)) {
        this.activeCommands[commandId]?.resolve([commandResponse])
      }
    } catch {
      // ignore, promise already resolved or rejected
    }
  }

  private getNextCommandId() {
    const commandId = this.nextCommandId++
    if (this.nextCommandId > 999) {
      this.nextCommandId = 0
    }
    return commandId
  }

  private formatPacket(commandId: number, type: string, args: string[]): CommandPacket {
    const encoder = new TextEncoder()
    let text = `${commandId}:${type}`
    if (args.length > 0) {
      text += `:${args.join(",")}`
    }
    const bytes = encoder.encode(text + "\n")
    return { text, bytes }
  }
}

type CommandPacket = {
  text: string
  bytes: Uint8Array
}