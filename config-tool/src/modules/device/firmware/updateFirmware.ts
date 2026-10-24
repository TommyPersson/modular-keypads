import type { DeviceFacade } from "@src/modules/device/facade/DeviceFacade"
import {
  AbortFirmwareUpdateDeviceCommand
} from "@src/modules/device/facade/device-commands/AbortFirmwareUpdateDeviceCommand"
import {
  BeginFirmwareUpdateDeviceCommand
} from "@src/modules/device/facade/device-commands/BeginFirmwareUpdateDeviceCommand"
import {
  FinishFirmwareUpdateDeviceCommand
} from "@src/modules/device/facade/device-commands/FinishFirmwareUpdateDeviceCommand"
import {
  WriteFirmwareUpdateDeviceCommand
} from "@src/modules/device/facade/device-commands/WriteFirmwareUpdateDeviceCommand"
import { validateFirmwareImage } from "./validateFirmwareImage"

// The device reads commands as lines of at most 1024 characters, which a chunk must fit in once Base64 encoded
const ChunkSize = 512

export type FirmwareUpdatePhase = "uploading" | "verifying" | "restarting" | "done"

export type FirmwareUpdateProgress = {
  phase: FirmwareUpdatePhase
  writtenBytes: number
  totalBytes: number
}

/**
 * Updates the connected device with the given application image, i.e. the firmware.bin built by PlatformIO, and
 * reconnects to it once it has restarted into the new firmware. Updates that fail or are aborted before they're
 * finished leave the current firmware in place.
 */
export async function updateFirmware(options: {
  facade: DeviceFacade
  image: Uint8Array
  onProgress: (progress: FirmwareUpdateProgress) => void
  abortSignal?: AbortSignal
}): Promise<void> {
  const { facade, image, onProgress, abortSignal } = options

  await validateFirmwareImage(image)

  const totalBytes = image.length

  onProgress({ phase: "uploading", writtenBytes: 0, totalBytes })

  await facade.executeCommand(new BeginFirmwareUpdateDeviceCommand(totalBytes))

  try {
    for (let offset = 0; offset < totalBytes; offset += ChunkSize) {
      abortSignal?.throwIfAborted()

      onProgress({ phase: "uploading", writtenBytes: offset, totalBytes })

      const chunk = image.subarray(offset, offset + ChunkSize)
      await facade.executeCommand(new WriteFirmwareUpdateDeviceCommand(offset, chunk))
    }

    abortSignal?.throwIfAborted()
  } catch (e) {
    // The device also discards it on its own when no more chunks arrive, in case this doesn't reach it
    await facade.executeCommand(new AbortFirmwareUpdateDeviceCommand()).catch(console.warn)
    throw e
  }

  onProgress({ phase: "verifying", writtenBytes: totalBytes, totalBytes })

  // Started before the restart, as the device can disconnect and reconnect quickly
  const restartAbortController = new AbortController()
  const restarted = facade.expectRestart(undefined, restartAbortController.signal)
  restarted.catch(() => {
    // awaited below, unless finishing fails
  })

  try {
    await facade.executeCommand(new FinishFirmwareUpdateDeviceCommand())
  } catch (e) {
    restartAbortController.abort()
    throw e
  }

  onProgress({ phase: "restarting", writtenBytes: totalBytes, totalBytes })

  await restarted

  onProgress({ phase: "done", writtenBytes: totalBytes, totalBytes })
}
