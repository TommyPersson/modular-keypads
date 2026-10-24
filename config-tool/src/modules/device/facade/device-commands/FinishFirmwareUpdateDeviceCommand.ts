import { DeviceCommand } from "./DeviceCommand"

/**
 * Verifies the written image and makes it the one to boot. The device restarts shortly after responding.
 */
export class FinishFirmwareUpdateDeviceCommand extends DeviceCommand<void> {
  override type: string = "finish.firmware.update"

  // The whole image is read back to verify it
  override get timeoutMs(): number {
    return 10000
  }

  override parseResponse(_: string[]): void {
  }
}
