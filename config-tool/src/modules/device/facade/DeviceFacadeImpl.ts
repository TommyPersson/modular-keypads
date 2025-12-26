import { createModifierFlags, type MacroDefinition, MacroDefinitionType } from "@src/modules/key-bindings/models"
import { readLines } from "@src/utils/streams"
import { DateTime } from "luxon"
import { BehaviorSubject, Observable, Subject } from "rxjs"
import { DeviceCommandExecutor } from "./DeviceCommandExecutor"
import type {
  DeviceCapability,
  DeviceFacade,
  DeviceInformation,
  DeviceRegisterValues,
  RawLogMessage,
  NotificationMessage
} from "./DeviceFacade"

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

  async performPing(): Promise<string> {
    return await this.sendSingleLineResponseCommand("ping", ["a", "b", "c"])
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
      deviceName: await this.getDeviceName(),
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

  async listConnectedDevices(): Promise<DeviceInformation[]> {
    const lines = await this.sendCommand("list.connected.devices")

    return lines.map(line => {
      const [idHex, addressHex, type, name] = line.split(",")
      return {
        deviceId: idHex,
        deviceFirmwareVersion: "0.0.1",
        deviceType: type,
        deviceAddress: parseInt(addressHex, 16),
        deviceName: name,
        deviceRegisterNames: [],
      } satisfies DeviceInformation
    })
  }

  async listDeviceCapabilities(deviceId: string): Promise<DeviceCapability[]> {
    const lines = await this.sendCommand("list.device.capabilities", [deviceId])

    return lines.map((line, index) => {
      const [type, ...rest] = line.split(",")
      if (type == "PushButton") {
        return {
          index,
          type,
          number: parseInt(rest[0])
        } satisfies DeviceCapability
      } else if (type == "RotaryEncoder") {
        return {
          index,
          type,
          number: parseInt(rest[0])
        } satisfies DeviceCapability
      } else {
        return null
      }
    }).filter(it => it) as DeviceCapability[]
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

  async setDeviceType(type: string) {
    await this.sendWriteCommand("set.device.type", [type])
  }

  async getDeviceAddress(): Promise<number> {
    const addressHex = await this.sendSingleLineResponseCommand("read.device.address")
    return parseInt(addressHex, 16)
  }

  async setDeviceAddress(address: number) {
    const addressHex = "0x" + address.toString(16)
    await this.sendWriteCommand("set.device.address", [addressHex])
  }

  async getDeviceName(): Promise<string> {
    return await this.sendSingleLineResponseCommand("read.device.name")
  }

  async setDeviceName(name: string) {
    await this.sendWriteCommand("set.device.name", [name])
  }

  private async sendWriteCommand(str: string, args: string[] = []): Promise<void> {
    await this.sendSingleLineResponseCommand(str, args)
  }

  async getDeviceRegisterNames(): Promise<string[]> {
    return await this.sendCommand("list.registers")
  }

  async getDeviceRegisterValue(register: string): Promise<number> {
    const addressHex = await this.sendSingleLineResponseCommand("read.register", [register])
    return parseInt(addressHex, 16)
  }

  async getTestMode(): Promise<boolean> {
    const state = await this.sendSingleLineResponseCommand("get.test.mode")
    return state == "true"
  }

  async setTestMode(enabled: boolean): Promise<void> {
    await this.sendCommand("set.test.mode", [enabled ? "true" : "false"])
  }

  async saveMacro(macro: MacroDefinition) {
    const dataArgs = (() => {
      if (macro.type === MacroDefinitionType.Shortcut) {
        return [
          "0x01", // Shortcut
          `0x${createModifierFlags(macro.shortcut.modifiers).toString(16).padStart(2, "0")}`,
          `0x${macro.shortcut.hidCode.toString(16).padStart(2, "0")}`
        ]
      } else {
        throw new Error("Unsupported macro type")
      }
    })()

    await this.sendCommand("save.macro", [
      macro.id.toString(),
      macro.name,
      ...dataArgs
    ])
  }

  private async sendCommand(str: string, args: string[] = []): Promise<string[]> {
    if (!this.commandExecutor) {
      throw new Error("Not connected")
    }

    return this.commandExecutor?.sendCommand(str, args)
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
