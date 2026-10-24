import { DeviceCommand } from "./DeviceCommand"

export class WriteFirmwareUpdateDeviceCommand extends DeviceCommand<void> {
  override type: string = "write.firmware.update"

  constructor(
    private readonly offset: number,
    private readonly chunk: Uint8Array,
  ) {
    super()
  }

  override get arguments(): string[] {
    return [this.offset.toString(), toBase64(this.chunk)]
  }

  // Chunks that complete a flash sector wait for it to be erased and written
  override get timeoutMs(): number {
    return 5000
  }

  override parseResponse(_: string[]): void {
  }
}

function toBase64(bytes: Uint8Array): string {
  let binary = ""
  for (const byte of bytes) {
    binary += String.fromCharCode(byte)
  }
  return btoa(binary)
}
