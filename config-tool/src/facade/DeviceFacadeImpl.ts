import { DateTime } from "luxon"
import { BehaviorSubject, Observable, Subject } from "rxjs"
import { readLines } from "../utils/streams"
import { DeviceCommandExecutor } from "./DeviceCommandExecutor"
import type { DeviceFacade, DeviceInformation, DeviceRegisterValues, LogMessage } from "./DeviceFacade"

export class DeviceFacadeImpl implements DeviceFacade {

  private logsSubject = new Subject<LogMessage>()
  private isConnectedSubject = new BehaviorSubject<boolean>(false)
  private _isConnected = false

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
    return await this.sendCommand("ping", ["a", "b", "c"])
  }

  async resetDevice(): Promise<void> {
    await this.sendWriteCommand("reset.device").catch(() => {
    })

    return
  }

  async getDeviceInformation(): Promise<DeviceInformation> {
    return {
      deviceId: await this.getDeviceId(),
      deviceFirmwareVersion: await this.getDeviceFirmwareVersion(),
      deviceType: await this.getDeviceType(),
      deviceAddress: await this.getDeviceAddress(),
      deviceRegisterNames: await this.getDeviceRegisterNames(),
    }
  }

  async getDeviceRegisterValues(): Promise<DeviceRegisterValues> {
    const registerNames = await this.getDeviceRegisterNames()

    const result: DeviceRegisterValues = {}

    for (const name of registerNames) {
      result[name] = await this.getDeviceRegisterValue(name)
    }

    return result
  }

  async getDeviceId(): Promise<string> {
    return await this.sendCommand("read.device.id")
  }

  async getDeviceFirmwareVersion(): Promise<string> {
    return await this.sendCommand("read.device.firmware.version")
  }

  async getDeviceType(): Promise<string> {
    return await this.sendCommand("read.device.type")
  }

  async setDeviceType(type: string) {
    await this.sendWriteCommand("set.device.type", [type])
  }

  async getDeviceAddress(): Promise<number> {
    const addressHex = await this.sendCommand("read.device.address")
    return parseInt(addressHex, 16)
  }

  async setDeviceAddress(address: number) {
    const addressHex = "0x" + address.toString(16)
    await this.sendWriteCommand("set.device.address", [addressHex])
  }

  private async sendWriteCommand(str: string, args: string[] = []): Promise<void> {
    const response = await this.sendCommand(str, args)
    if (response.toLowerCase() !== "ack") {
      throw new Error(`Unable to reset device: ${response}`)
    }
  }

  async getDeviceRegisterNames(): Promise<string[]> {
    const response = await this.sendCommand("list.registers")
    return response.split(",")
  }

  async getDeviceRegisterValue(register: string): Promise<number> {
    const addressHex = await this.sendCommand("read.register", [register])
    return parseInt(addressHex, 16)
  }

  private async sendCommand(str: string, args: string[] = []): Promise<string> {
    if (!this.commandExecutor) {
      throw new Error("Not connected")
    }

    return this.commandExecutor?.sendCommand(str, args)
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

  get $logs(): Observable<LogMessage> {
    return this.logsSubject
  }

  get $isConnected(): Observable<boolean> {
    return this.isConnectedSubject
  }

  get isConnected(): boolean {
    return this._isConnected
  }
}
