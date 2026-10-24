import { Deferred } from "@src/utils/Deferred"
import { Mutex } from "@src/utils/Mutex"
import { DateTime } from "luxon"
import { Subject } from "rxjs"
import { DeviceCommandError, type DeviceMessage } from "../models"

export class DeviceCommandExecutor {

  private activeCommands: { [index: number]: Deferred<string[]> } = {}
  private activeCommandResponses: Map<number, string[]> = new Map()
  private nextCommandId: number = 1

  private readonly sendMutex: Mutex = new Mutex()

  private readonly commandTimeoutMs = 2000
  private readonly mutexTimeoutMs = this.commandTimeoutMs + 1000

  // Long commands, like firmware chunks, are cut off in the traffic, as they'd only make it hard to read
  private readonly maxTrafficMessageLength = 200

  constructor(
    private readonly writer: WritableStreamDefaultWriter,
    private readonly logsSubject: Subject<DeviceMessage>,
  ) {
  }

  async sendCommand(type: string, args: string[], timeoutMs: number = this.commandTimeoutMs): Promise<string[]> {
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

      const trafficText = packet.text.length > this.maxTrafficMessageLength
        ? `${packet.text.substring(0, this.maxTrafficMessageLength)}...`
        : packet.text

      console.debug(`Sending packet: '${trafficText}'`)

      await writer.write(packet.bytes)

      this.logsSubject.next({ direction: "to-device", message: trafficText, timestamp: DateTime.now() })

      const timeout = setTimeout(() => {
        deferredCommandResult.reject(new Error("command timeout"))
      }, timeoutMs)

      const response = await deferredCommandResult.promise

      clearTimeout(timeout)

      return response
    } finally {
      delete this.activeCommands[commandId]
      this.sendMutex.release()
    }
  }

  onLineReceived(line: string) {
    console.debug("onLineReceived", line)

    if (!line.startsWith("%")) {
      return
    }


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
        if (sequenceNumberStr == "e") {
          this.activeCommands[commandId]?.reject(new DeviceCommandError(commandId, commandResponse));
        } else {
          this.activeCommands[commandId]?.resolve([commandResponse])
        }
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