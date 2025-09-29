import { DateTime } from "luxon"
import { BehaviorSubject, Observable, Subject } from "rxjs"
import { readLines } from "../utils/streams"
import { DeviceCommandExecutor } from "./DeviceCommandExecutor"
import type { DeviceFacade, LogMessage } from "./DeviceFacade"

export class DeviceFacadeImpl implements DeviceFacade {

  private logsSubject = new Subject<LogMessage>()
  private isConnectedSubject = new BehaviorSubject<boolean>(false)

  private notifications: string[] = []

  private writer: WritableStreamDefaultWriter<Uint8Array> | null = null

  private disconnectAbortController: AbortController | null = null

  private port: SerialPort | null = null

  private commandExecutor: DeviceCommandExecutor | null = null

  constructor() {
  }

  async connect(): Promise<void> {
    if (this.port?.connected) {
      return
    }

    try {
      await navigator.serial.getPorts()

      const abortController = new AbortController()
      this.disconnectAbortController = abortController

      this.port = await navigator.serial.requestPort({})
      this.port.open({ baudRate: 115200 }).then(async () => {
        await this.port!.setSignals({ dataTerminalReady: true, requestToSend: false }) // Avoids resetting the MC when connecting


        this.writer = this.port!.writable!.getWriter()
        this.commandExecutor = new DeviceCommandExecutor(this.writer, this.logsSubject)

        this.isConnectedSubject.next(true)

        try {
          if (this.port?.readable) {
            for await (const line of readLines(this.port?.readable, abortController.signal)) {
              if (!this.port.connected) {
                break
              }

              this.logsSubject.next({ direction: "to-host", message: line, timestamp: DateTime.now() })

              this.commandExecutor.onLineReceived(line)

              if (line.startsWith("!")) {
                this.notifications.push(line)
              }
            }
          }
        } catch (e) {
          console.error(e)
        } finally {
          await this.close()
        }
      })
    } catch (e) {
      console.error(e)
      await this.close()
    }
  }

  async disconnect(): Promise<void> {
    this.disconnectAbortController?.abort("disconnecting")
  }

  async performPing(): Promise<string> {
    return await this.sendCommand("ping")
  }

  private async sendCommand(str: string): Promise<string> {
    if (!this.commandExecutor) {
      throw new Error("Not connected")
    }

    return this.commandExecutor?.sendCommand(str)
  }

  private async close() {
    try {
      this.writer?.releaseLock()
      await this.writer?.close().catch(console.warn)
      await this.port?.close().catch(console.warn)
    } catch (e) {
      console.error("unable to close", e)
    }

    this.port = null
    this.disconnectAbortController = null
    this.commandExecutor = null
    this.isConnectedSubject.next(false)
  }

  get $logs(): Observable<LogMessage> {
    return this.logsSubject
  }

  get $isConnected(): Observable<boolean> {
    return this.isConnectedSubject
  }
}
