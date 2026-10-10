import type { DeviceFacade } from "@src/modules/device/facade"
import type { DeviceCommand } from "@src/modules/device/facade/device-commands/DeviceCommand"
import type { DeviceInformation, DeviceMessage, LogMessage, NotificationMessage } from "@src/modules/device/models"
import { DateTime } from "luxon"
import { BehaviorSubject, defer, from, merge, NEVER, Observable, Subject } from "rxjs"

export type MockNotification = {
  deviceId: string
  type: string
  args: string[]
  // Emitted again at this interval, for states that decay, like rotary encoder rotations
  repeatMs?: number
}

export type MockCommandHandler = (command: DeviceCommand<unknown>) => unknown

export type MockDeviceOptions = {
  // All devices, both the connected one and its sub-devices
  devices: DeviceInformation[]
  // The device the tool is connected to, defaults to the first one
  connectedDeviceId?: string
  notifications?: MockNotification[]
  // Responses to commands by command type, in addition to the built-in ones
  commandHandlers?: Record<string, MockCommandHandler>
}

/**
 * A device facade for Storybook, that is always connected and answers commands from the given options.
 *
 * Notifications without a repeat interval are replayed to every subscriber, so that components that mount late,
 * e.g. after a query has loaded, still get the same state as the others.
 */
export class MockDeviceFacade implements DeviceFacade {
  private readonly liveNotifications = new Subject<NotificationMessage>()
  private timers: ReturnType<typeof setInterval>[] = []

  readonly messages$: Observable<DeviceMessage> = NEVER
  readonly logs$: Observable<LogMessage[]> = NEVER
  readonly notifications$: Observable<NotificationMessage>
  readonly logs: ReadonlyArray<LogMessage> = []

  readonly isConnected$ = new BehaviorSubject(true)
  readonly isConnected = true

  constructor(private readonly options: MockDeviceOptions) {
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

  async connect(): Promise<void> {
  }

  async disconnect(): Promise<void> {
  }

  async getDeviceInformation(): Promise<DeviceInformation> {
    const { devices, connectedDeviceId } = this.options
    const device = connectedDeviceId ? devices.find(it => it.deviceId === connectedDeviceId) : devices[0]
    if (!device) {
      throw new Error("The mock device has no connected device")
    }
    return device
  }

  async executeCommand<TResult>(command: DeviceCommand<TResult>): Promise<TResult> {
    const handler = this.options.commandHandlers?.[command.type]
    if (handler) {
      return handler(command) as TResult
    }

    if (command.type === "list.connected.devices") {
      return this.options.devices as TResult
    }

    throw new Error(`The command '${command.type}' isn't mocked`)
  }

  clearLogs(): void {
  }

  deleteLogMessage(_: number): void {
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
