import type { DeviceFacade } from "@src/modules/device/facade"
import type {
  ClearKeyBindingDeviceCommand
} from "@src/modules/device/facade/device-commands/ClearKeyBindingDeviceCommand"
import type { DeviceCommand } from "@src/modules/device/facade/device-commands/DeviceCommand"
import type { SetKeyBindingDeviceCommand } from "@src/modules/device/facade/device-commands/SetKeyBindingDeviceCommand"
import type {
  DeviceCapability,
  DeviceInformation,
  DeviceMessage,
  DeviceMetric,
  LogMessage,
  NotificationMessage
} from "@src/modules/device/models"
import {
  type KeyBinding,
  KeyBindingTriggerType,
  PushButtonEvent,
  RotaryEncoderDirection
} from "@src/modules/key-bindings/models"
import { ListKeyBindingsQueryKey } from "@src/modules/key-bindings/queries"
import type { MacroDefinition } from "@src/modules/macros/models"
import { ListStoredMacrosQueryKey } from "@src/modules/macros/queries"
import type { Setting } from "@src/modules/settings/models"
import { ListSettingsQueryKey } from "@src/modules/settings/queries"
import { dequal } from "dequal"
import { DateTime } from "luxon"
import { BehaviorSubject, defer, from, merge, Observable, Subject } from "rxjs"

export type MockNotification = {
  deviceId: string
  type: string
  args: string[]
  // Emitted again at this interval, for states that decay, like rotary encoder rotations
  repeatMs?: number
}

export type MockCommandHandler = (command: DeviceCommand<unknown>) => unknown

export type MockLogMessage = Pick<LogMessage, "level" | "component" | "message">

export type MockDeviceOptions = {
  // Whether the tool starts out connected to the device, defaults to true
  connected?: boolean
  // All devices, both the connected one and its sub-devices
  devices: DeviceInformation[]
  // The device the tool is connected to, defaults to the first one
  connectedDeviceId?: string
  notifications?: MockNotification[]
  keyBindings?: KeyBinding[]
  macros?: MacroDefinition[]
  settings?: Setting[]
  metrics?: DeviceMetric[]
  // Log messages the device has sent, as if received right before the story was shown
  logs?: MockLogMessage[]
  // By device ID
  capabilities?: Record<string, DeviceCapability[]>
  // Responses to commands by command type, in addition to the built-in ones
  commandHandlers?: Record<string, MockCommandHandler>
}

/**
 * A device facade for Storybook, that answers commands from the given options. Renaming a device changes the listed
 * devices, and setting key bindings, saving macros and changing settings change the listed ones. Identifying a device
 * or a button does nothing. Sub-devices can be connected and disconnected, to see how the components react, and the
 * tool can connect to and disconnect from the device. The commands and their responses are shown as serial traffic.
 *
 * Notifications without a repeat interval are replayed to every subscriber, so that components that mount late,
 * e.g. after a query has loaded, still get the same state as the others.
 */
export class MockDeviceFacade implements DeviceFacade {
  private readonly liveNotifications = new Subject<NotificationMessage>()
  private timers: ReturnType<typeof setInterval>[] = []
  private devices: DeviceInformation[]
  private keyBindings: KeyBinding[]
  private macros: MacroDefinition[]
  private settings: Setting[]
  private testMode = false
  private nextCommandId = 1
  private readonly trafficSubject = new BehaviorSubject<DeviceMessage[]>([])
  private readonly logsSubject: BehaviorSubject<LogMessage[]>

  // Called when a command changes the data of the queries with the given key, so that they're refreshed
  onQueryDataChanged: (queryKey: readonly unknown[]) => void = () => {}

  readonly messages$ = new Subject<DeviceMessage>()
  readonly notifications$: Observable<NotificationMessage>
  readonly isConnected$: BehaviorSubject<boolean>

  constructor(private readonly options: MockDeviceOptions) {
    this.devices = options.devices
    this.keyBindings = options.keyBindings ?? []
    this.macros = options.macros ?? []
    this.settings = options.settings ?? []
    this.isConnected$ = new BehaviorSubject(options.connected ?? true)
    this.logsSubject = new BehaviorSubject((options.logs ?? []).map((it, index) => ({
      ...it,
      key: index,
      timestamp: DateTime.now(),
    })))
    const replayed = (options.notifications ?? []).filter(it => it.repeatMs === undefined)
    this.notifications$ = merge(
      defer(() => from(replayed.map(toNotificationMessage))),
      this.liveNotifications,
    )
  }

  // Starts the repeating notifications, until stopped
  start() {
    this.stop()
    this.timers = (this.options.notifications ?? [])
      .filter(it => it.repeatMs !== undefined)
      .map(it => setInterval(() => this.notify(it), it.repeatMs))
  }

  stop() {
    this.timers.forEach(clearInterval)
    this.timers = []
  }

  notify(notification: MockNotification) {
    this.liveNotifications.next(toNotificationMessage(notification))
  }

  get isConnected(): boolean {
    return this.isConnected$.value
  }

  async connect(): Promise<void> {
    this.isConnected$.next(true)
  }

  async disconnect(): Promise<void> {
    this.isConnected$.next(false)
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
    return this.logsSubject.value
  }

  clearLogs(): void {
    this.logsSubject.next([])
  }

  deleteLogMessage(key: number): void {
    this.logsSubject.next(this.logs.filter(it => it.key !== key))
  }

  async getDeviceInformation(): Promise<DeviceInformation> {
    const { connectedDeviceId } = this.options
    const devices = this.devices
    const device = connectedDeviceId ? devices.find(it => it.deviceId === connectedDeviceId) : devices[0]
    if (!device) {
      throw new Error("The mock device has no connected device")
    }
    return device
  }

  // Shown as serial traffic in the format of the real device
  async executeCommand<TResult>(command: DeviceCommand<TResult>): Promise<TResult> {
    const commandId = this.nextCommandId++
    const args = command.arguments
    this.addTraffic("to-device", `${commandId}:${command.type}${args.length > 0 ? `:${args.join(",")}` : ""}`)

    try {
      const result = this.respond(command)
      this.addTraffic("to-host", command.type === "ping" ? `%${commandId}:${result}` : `%${commandId}.0:`)
      return result
    } catch (error) {
      this.addTraffic("to-host", `%${commandId}.e:${(error as Error).message}`)
      throw error
    }
  }

  private addTraffic(direction: DeviceMessage["direction"], message: string) {
    const deviceMessage = { direction, message, timestamp: DateTime.now() }
    this.messages$.next(deviceMessage)
    this.trafficSubject.next([...this.traffic, deviceMessage])
  }

  private respond<TResult>(command: DeviceCommand<TResult>): TResult {
    const handler = this.options.commandHandlers?.[command.type]
    if (handler) {
      return handler(command) as TResult
    }

    switch (command.type) {
      case "list.connected.devices":
        return this.devices as TResult
      case "set.device.name":
        this.renameDevice(command.arguments[0], command.arguments[1])
        return undefined as TResult
      case "flash.identification.lights":
      case "flash.button.identification.light":
        return undefined as TResult
      case "list.device.capabilities":
        return (this.options.capabilities?.[command.arguments[0]] ?? []) as TResult
      case "list.key.bindings":
        return this.keyBindings as TResult
      case "set.key.binding": {
        const { trigger, macroId } = command as unknown as SetKeyBindingDeviceCommand
        this.changeKeyBindings([...this.keyBindings.filter(it => !dequal(it.trigger, trigger)), { trigger, macroId }])
        return undefined as TResult
      }
      case "clear.key.binding": {
        const { trigger } = command as unknown as ClearKeyBindingDeviceCommand
        this.changeKeyBindings(this.keyBindings.filter(it => !dequal(it.trigger, trigger)))
        return undefined as TResult
      }
      case "list.stored.macros":
        return this.macros as TResult
      case "save.macro": {
        const { macro } = command as unknown as { macro: MacroDefinition }
        // New macros get the ID after the highest one, as on the device
        const id = macro.id > 0 ? macro.id : Math.max(0, ...this.macros.map(it => it.id)) + 1
        this.changeMacros([...this.macros.filter(it => it.id !== id), { ...macro, id }])
        return id as TResult
      }
      case "delete.macro":
        this.changeMacros(this.macros.filter(it => it.id.toString() !== command.arguments[0]))
        return undefined as TResult
      case "list.settings":
        return this.settings as TResult
      case "set.setting": {
        const [key, value] = command.arguments
        this.settings = this.settings.map(it => it.key === key ? { ...it, value: parseInt(value, 10) } : it)
        this.onQueryDataChanged(ListSettingsQueryKey)
        return undefined as TResult
      }
      case "read.metrics":
        return (this.options.metrics ?? []) as TResult
      case "ping":
        return "pong" as TResult
      case "get.test.mode":
        return this.testMode as TResult
      case "set.test.mode":
        this.testMode = command.arguments[0] === "true"
        return undefined as TResult
    }

    throw new Error(`The command '${command.type}' isn't mocked`)
  }

  // The commands refresh the queries of the app, not the queries of the story, which are refreshed here instead
  private changeKeyBindings(keyBindings: KeyBinding[]) {
    this.keyBindings = keyBindings
    this.onQueryDataChanged(ListKeyBindingsQueryKey)
  }

  private changeMacros(macros: MacroDefinition[]) {
    this.macros = macros
    this.onQueryDataChanged(ListStoredMacrosQueryKey)
  }

  // Notifies as the device does, which makes the device list refresh
  connectDevice(device: DeviceInformation) {
    this.devices = [...this.devices.filter(it => it.deviceId !== device.deviceId), device]
    this.notify({ deviceId: device.deviceId, type: "device.connected", args: [] })
  }

  // Including its sub-devices, which are connected through it
  disconnectDevice(deviceId: string) {
    this.devices = this.devices.filter(it => it.deviceId !== deviceId && it.parentDeviceId !== deviceId)
    this.notify({ deviceId, type: "device.disconnected", args: [] })
  }

  private renameDevice(deviceId: string, deviceName: string) {
    this.devices = this.devices.map(it => it.deviceId === deviceId ? { ...it, deviceName } : it)
    // As sent by the device, which makes the device list refresh
    this.notify({ deviceId, type: "device.renamed", args: [] })
  }
}

function toNotificationMessage(notification: MockNotification): NotificationMessage {
  const { deviceId, type, args } = notification
  return { deviceId, type, args, timestamp: DateTime.now() }
}

export function mockDevice(
  device: Pick<DeviceInformation, "deviceId" | "deviceType"> & Partial<DeviceInformation>
): DeviceInformation {
  return {
    deviceFirmwareVersion: "0.0.1",
    deviceAddress: 0,
    deviceName: `Device ${device.deviceId}`,
    deviceRegisterNames: [],
    parentDeviceId: null,
    subDeviceSlot: null,
    ...device,
  }
}

export const mockNotifications = {
  pressed: (deviceId: string, number: number): MockNotification =>
    ({ deviceId, type: "switch.pressed", args: [number.toString()] }),
  longPressed: (deviceId: string, number: number): MockNotification =>
    ({ deviceId, type: "switch.long-pressed", args: [number.toString()] }),
  // Repeated faster than the rotation indicators decay, so that they stay lit
  rotatingClockwise: (deviceId: string, number: number): MockNotification =>
    ({ deviceId, type: "encoder.rotated.clockwise", args: [number.toString()], repeatMs: 200 }),
  rotatingCounterClockwise: (deviceId: string, number: number): MockNotification =>
    ({ deviceId, type: "encoder.rotated.counter-clockwise", args: [number.toString()], repeatMs: 200 }),
}

export const mockKeyBindings = {
  pressed: (deviceId: string, number: number, macroId: number): KeyBinding =>
    ({ macroId, trigger: { type: KeyBindingTriggerType.PushButton, deviceId, number, event: PushButtonEvent.Pressed } }),
  longPressed: (deviceId: string, number: number, macroId: number): KeyBinding =>
    ({ macroId, trigger: { type: KeyBindingTriggerType.PushButton, deviceId, number, event: PushButtonEvent.LongPressed } }),
  clockwise: (deviceId: string, number: number, macroId: number): KeyBinding => ({
    macroId,
    trigger: { type: KeyBindingTriggerType.RotaryEncoder, deviceId, number, direction: RotaryEncoderDirection.Clockwise },
  }),
  counterClockwise: (deviceId: string, number: number, macroId: number): KeyBinding => ({
    macroId,
    trigger: {
      type: KeyBindingTriggerType.RotaryEncoder,
      deviceId,
      number,
      direction: RotaryEncoderDirection.CounterClockwise,
    },
  }),
}
