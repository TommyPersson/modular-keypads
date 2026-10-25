import type { DeviceCommand } from "@src/modules/device/facade/device-commands/DeviceCommand"
import { takeLast } from "@src/utils/arrays"
import { readLines } from "@src/utils/streams"
import { DateTime } from "luxon"
import { BehaviorSubject, Observable, Subject, Subscription } from "rxjs"
import type { DeviceInformation, DeviceMessage, LogMessage, NotificationMessage } from "../models"
import { DeviceCommandExecutor } from "./DeviceCommandExecutor"
import type { DeviceFacade } from "./DeviceFacade"

const MaxTrafficMessages = 1000

export class DeviceFacadeImpl implements DeviceFacade {

  private messagesSubject = new Subject<DeviceMessage>()
  private logsSubject = new Subject<LogMessage[]>()
  private trafficSubject = new BehaviorSubject<DeviceMessage[]>([])
  private notificationsSubject = new Subject<NotificationMessage>()

  private _logs: LogMessage[] = []

  private isConnectedSubject = new BehaviorSubject<boolean>(false)
  private _isConnected = false

  private writer: WritableStreamDefaultWriter<Uint8Array> | null = null

  private disconnectAbortController: AbortController | null = null

  private port: SerialPort | null = null

  // The port to reconnect to when it's reported again, e.g. after the device has been reset. Cleared when the user
  // disconnects.
  private reconnectPortInfo: SerialPortInfo | null = null

  private commandExecutor: DeviceCommandExecutor | null = null

  private nextLogMessageKey = 0

  constructor() {
    this.messagesSubject.subscribe(message => {
      this.trafficSubject.next([...takeLast(this.trafficSubject.value, MaxTrafficMessages - 1), message])
    })

    // Not available in all browsers
    if ("serial" in navigator) {
      navigator.serial.addEventListener("connect", event => this.onPortConnected(event.target as SerialPort))
    }
  }

  async connect(): Promise<void> {
    if (this.port?.connected) {
      return
    }

    try {
      await navigator.serial.getPorts()

      const port = await navigator.serial.requestPort({})
      await this.openPort(port)
    } catch (e) {
      console.error(e)
      this.reconnectPortInfo = null
      await this.close()
    }
  }

  // The device is reconnected to like after any other reset, so this only waits for it to disconnect and reconnect
  async expectRestart(timeoutMs: number = 30_000, abortSignal?: AbortSignal): Promise<void> {
    let subscription: Subscription | null = null
    let timeout: ReturnType<typeof setTimeout> | null = null

    const reconnected = new Promise<void>((resolve, reject) => {
      timeout = setTimeout(() => reject(new Error("The device didn't reconnect after restarting")), timeoutMs)

      let disconnected = false
      subscription = this.isConnectedSubject.subscribe(isConnected => {
        if (!isConnected) {
          disconnected = true
        } else if (disconnected) {
          resolve()
        }
      })

      abortSignal?.addEventListener("abort", () => reject(abortSignal.reason))
    })

    return reconnected.finally(() => {
      subscription!.unsubscribe()
      clearTimeout(timeout!)
    })
  }

  // Only the previously granted port is reported again, which is opened without asking the user to select it.
  // Failures are left silent, as the user can still connect manually.
  private async onPortConnected(port: SerialPort) {
    if (this.port?.connected || !this.reconnectPortInfo) {
      return
    }

    const portInfo = port.getInfo()
    if (portInfo.usbVendorId !== this.reconnectPortInfo.usbVendorId
      || portInfo.usbProductId !== this.reconnectPortInfo.usbProductId) {
      return
    }

    // The previous port may not have noticed that it was disconnected yet
    if (this.port) {
      await this.close()
    }

    try {
      await this.openPort(port)
    } catch (e) {
      console.warn("Unable to reconnect", e)
      await this.close(port)
    }
  }

  // Resolves once the port is open, while its lines are read until it's closed or disconnected
  private async openPort(port: SerialPort): Promise<void> {
    const abortController = new AbortController()
    this.disconnectAbortController = abortController

    this.port = port
    this.reconnectPortInfo = port.getInfo()
    await port.open({ baudRate: 115200 })

    this.writer = port.writable!.getWriter()

    // Ends any line that was cut off when the tool last disconnected, e.g. in the middle of a firmware chunk, which the
    // device would otherwise prepend to the first command sent now
    await this.writer.write(new TextEncoder().encode("\n"))

    this.commandExecutor = new DeviceCommandExecutor(this.writer, this.messagesSubject)

    this._isConnected = true
    this.isConnectedSubject.next(true)

    this.readLines(port, abortController.signal).then()
  }

  private async readLines(port: SerialPort, abortSignal: AbortSignal): Promise<void> {
    try {
      if (port.readable) {
        for await (const line of readLines(port.readable, abortSignal)) {
          if (!port.connected) {
            break
          }

          const deviceMessage: DeviceMessage = { direction: "to-host", message: line, timestamp: DateTime.now() }
          this.messagesSubject.next(deviceMessage)

          this.commandExecutor?.onLineReceived(line)

          if (line.startsWith("!")) {
            const [deviceId, type, ...args] = line.substring(1).split(":")
            this.notificationsSubject.next({ deviceId, type, args, timestamp: DateTime.now() })
          }

          if (line.startsWith("#")) {
            const [level, component, ...messageParts] = line.substring(1).split(":")
            const message = messageParts.length > 0 ? messageParts.join(":") : null

            const logMessage = {
              key: this.nextLogMessageKey++,
              timestamp: DateTime.now(),
              level,
              component: message ? component : null,
              message: message ? message : component
            }

            this._logs = [...takeLast(this._logs, 999), logMessage]
            this.logsSubject.next(this._logs)
          }
        }
      }
    } catch (e) {
      console.error(e)
    } finally {
      await this.close(port)
    }
  }

  async disconnect(): Promise<void> {
    this.reconnectPortInfo = null
    this.disconnectAbortController?.abort("disconnecting")
  }

  async getDeviceInformation(): Promise<DeviceInformation> {
    return {
      deviceId: await this.getDeviceId(),
      deviceFirmwareVersion: await this.getDeviceFirmwareVersion(),
      deviceType: await this.getDeviceType(),
      // The connected device is the master, which has no address
      deviceAddress: 0,
      deviceName: await this.getDeviceName(),
      deviceRegisterNames: await this.getDeviceRegisterNames(),
      parentDeviceId: null,
      subDeviceSlot: null,
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

  async getDeviceName(): Promise<string> {
    return await this.sendSingleLineResponseCommand("read.device.name")
  }

  async getDeviceRegisterNames(): Promise<string[]> {
    return await this.sendCommand("list.registers")
  }

  async executeCommand<TResult>(command: DeviceCommand<TResult>): Promise<TResult> {
    const type = command.type
    const args = command.arguments

    const response = await this.sendCommand(type, args, command.timeoutMs)

    return command.parseResponse(response)
  }

  private async sendCommand(type: string, args: string[] = [], timeoutMs?: number): Promise<string[]> {
    if (!this.commandExecutor) {
      throw new Error("Not connected")
    }

    return this.commandExecutor?.sendCommand(type, args, timeoutMs)
  }

  private async sendSingleLineResponseCommand(str: string, args: string[] = []): Promise<string> {
    if (!this.commandExecutor) {
      throw new Error("Not connected")
    }

    return (await this.commandExecutor.sendCommand(str, args))[0]
  }

  // Only the current port is closed, as a port that disconnected may finish closing after it has been reopened. The
  // state is reset before the port is closed, so that a port opened in the meantime isn't reset by it.
  private async close(port: SerialPort | null = this.port) {
    if (port !== this.port) {
      return
    }

    const writer = this.writer

    this.port = null
    this.writer = null
    this.disconnectAbortController = null
    this.commandExecutor = null
    this._isConnected = false
    this.isConnectedSubject.next(false)

    try {
      writer?.releaseLock()
      await writer?.close().catch(console.warn)
      await port?.close().catch(console.warn)
    } catch (e) {
      console.error("unable to close", e)
    }
  }

  get messages$(): Observable<DeviceMessage> {
    return this.messagesSubject
  }

  get traffic$(): Observable<DeviceMessage[]> {
    return this.trafficSubject
  }

  get traffic(): ReadonlyArray<DeviceMessage> {
    return this.trafficSubject.value
  }

  clearTraffic(): void {
    this.trafficSubject.next([])
  }

  get logs$(): Observable<LogMessage[]> {
    return this.logsSubject
  }

  get logs(): ReadonlyArray<LogMessage> {
    return this._logs
  }

  clearLogs(): void {
    this._logs = []
    this.logsSubject.next(this._logs)
  }

  deleteLogMessage(key: number) {
    this._logs = this._logs.filter(it => it.key !== key)
    this.logsSubject.next(this._logs)
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
