import type { DeviceCommand } from "@src/modules/device/facade/device-commands/DeviceCommand"
import { readLines } from "@src/utils/streams"
import { DateTime } from "luxon"
import { BehaviorSubject, Observable, Subject } from "rxjs"
import type { DeviceInformation, NotificationMessage, RawLogMessage } from "../models"
import { DeviceCommandExecutor } from "./DeviceCommandExecutor"
import type { DeviceFacade } from "./DeviceFacade"

export class DeviceFacadeImpl implements DeviceFacade {

  private logsSubject = new Subject<RawLogMessage>()
  private notificationsSubject = new Subject<NotificationMessage>()
  private isConnectedSubject = new BehaviorSubject<boolean>(false)
  private _isConnected = false

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
        //await this.port!.setSignals({ dataTerminalReady: true, requestToSend: false }) // Avoids resetting the MC when connecting


        this.writer = this.port!.writable!.getWriter()
        this.commandExecutor = new DeviceCommandExecutor(this.writer, this.logsSubject)

        this._isConnected = true
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
                const [deviceId, type, ...args] = line.substring(1).split(":")
                this.notificationsSubject.next({ deviceId, type, args, timestamp: DateTime.now() })
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

  async getDeviceInformation(): Promise<DeviceInformation> {
    return {
      deviceId: await this.getDeviceId(),
      deviceFirmwareVersion: await this.getDeviceFirmwareVersion(),
      deviceType: await this.getDeviceType(),
      deviceAddress: await this.getDeviceAddress(),
      deviceName: await this.getDeviceName(),
      deviceRegisterNames: await this.getDeviceRegisterNames(),
    }
  }

  async getDeviceId(): Promise<string> {
    return await this.sendSingleLineResponseCommand("read.device.id")
  }

  async getDeviceFirmwareVersion(): Promise<string> {
    return await this.sendSingleLineResponseCommand("read.device.firmware.version")
  }

  async getDeviceType(): Promise<string> {
    return await this.sendSingleLineResponseCommand("read.device.type")
  }

  async getDeviceAddress(): Promise<number> {
    const addressHex = await this.sendSingleLineResponseCommand("read.device.address")
    return parseInt(addressHex, 16)
  }

  async getDeviceName(): Promise<string> {
    return await this.sendSingleLineResponseCommand("read.device.name")
  }

  async getDeviceRegisterNames(): Promise<string[]> {
    return await this.sendCommand("list.registers")
  }

  async executeCommand<TResult>(command: DeviceCommand<TResult>): Promise<TResult> {
    const type = command.type
    const args = command.arguments

    const response = await this.sendCommand(type, args)

    return command.parseResponse(response)
  }

  private async sendCommand(type: string, args: string[] = []): Promise<string[]> {
    if (!this.commandExecutor) {
      throw new Error("Not connected")
    }

    return this.commandExecutor?.sendCommand(type, args)
  }

  private async sendSingleLineResponseCommand(str: string, args: string[] = []): Promise<string> {
    if (!this.commandExecutor) {
      throw new Error("Not connected")
    }

    return (await this.commandExecutor.sendCommand(str, args))[0]
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
    this._isConnected = false
    this.isConnectedSubject.next(false)
  }

  get logs$(): Observable<RawLogMessage> {
    return this.logsSubject
  }

  get notifications$(): Observable<NotificationMessage> {
    return this.notificationsSubject
  }

  get isConnected$(): Observable<boolean> {
    return this.isConnectedSubject
  }

  get isConnected(): boolean {
    return this._isConnected
  }
}
