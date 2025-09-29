import { DateTime } from "luxon"
import { Subject } from "rxjs"
import { Deferred } from "../utils/Deferred"
import { Mutex } from "../utils/Mutex"
import type { LogMessage } from "./DeviceFacade"

export class DeviceCommandExecutor {

  private activeCommands: { [index: number]: Deferred<string> } = {}
  private nextCommandId: number = 1

  private readonly sendMutex: Mutex = new Mutex()

  private readonly commandTimeoutMs = 2000
  private readonly mutexTimeoutMs = this.commandTimeoutMs + 1000

  constructor(
    private readonly writer: WritableStreamDefaultWriter,
    private readonly logsSubject: Subject<LogMessage>,
  ) {
  }

  async sendCommand(str: string): Promise<string> {
    const writer = this.writer
    if (!writer) {
      throw new Error("Not connected")
    }

    const commandId = this.getNextCommandId()
    const deferredCommandResult = new Deferred<string>()
    this.activeCommands[commandId] = deferredCommandResult

    try {
      await this.sendMutex.acquire(this.mutexTimeoutMs)

      const packet = this.formatPacket(commandId, str)

      this.logsSubject.next({ direction: "to-device", message: packet.text, timestamp: DateTime.now() })

      await writer.write(packet.bytes)

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

    try {
      const [commandIdStr, ...rest] = line.substring(1).split(":")
      const commandId = parseInt(commandIdStr)
      const commandResponse = rest.join(":")
      this.activeCommands[commandId]?.resolve(commandResponse)
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

  private formatPacket(commandId: number, str: string): CommandPacket {
    const encoder = new TextEncoder()
    const text = `${commandId}:${str}`
    const bytes = encoder.encode(text + "\n")
    return { text, bytes }
  }
}

type CommandPacket = {
  text: string
  bytes: Uint8Array
}