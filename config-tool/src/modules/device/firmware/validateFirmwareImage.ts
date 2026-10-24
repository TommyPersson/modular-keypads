/**
 * Checks that a file is an application image for the modules, i.e. the firmware.bin built by PlatformIO, before it's
 * uploaded. The build folder has other .bin files that are easily picked by mistake: bootloader.bin is an ESP32 image
 * too, but not an application, and partitions.bin is the partition table.
 *
 * The device only finds out about a wrong or damaged image once all of it has been uploaded, and leaves the current
 * firmware in place then, so this is about clear errors early rather than about safety.
 *
 * See https://docs.espressif.com/projects/esptool/en/latest/esp32s3/advanced-topics/firmware-image-format.html
 */

export class FirmwareImageError extends Error {
}

// The first byte of every ESP32 image, applications and bootloaders alike
const ImageMagic = 0xe9

// Partition tables start with the magic bytes of their first entry
const PartitionTableMagic = [0xaa, 0x50]

// The chip the image is built for, in its extended header
const ChipIdOffset = 12
const Esp32S3ChipId = 0x0009

// Whether a SHA-256 of the whole image is appended to it, in its extended header
const HashAppendedOffset = 23
const HashLength = 32

// Applications start their first segment, after the 24 byte image header and the 8 byte segment header, with an
// application description, which bootloaders don't have
const AppDescriptionOffset = 32
const AppDescriptionMagic = 0xabcd5432

const ImageHeaderLength = 24

const chipNames: Record<number, string> = {
  0x0000: "ESP32",
  0x0002: "ESP32-S2",
  0x0005: "ESP32-C3",
  0x0009: "ESP32-S3",
  0x000c: "ESP32-C2",
  0x000d: "ESP32-C6",
  0x0010: "ESP32-H2",
  0x0012: "ESP32-P4",
}

export async function validateFirmwareImage(image: Uint8Array): Promise<void> {
  if (image.length >= 2 && image[0] === PartitionTableMagic[0] && image[1] === PartitionTableMagic[1]) {
    throw new FirmwareImageError("This is the partition table (partitions.bin), not the firmware. Select firmware.bin.")
  }

  if (image.length < AppDescriptionOffset + 4 || image[0] !== ImageMagic) {
    throw new FirmwareImageError("This isn't an ESP32 firmware image. Select the firmware.bin built by PlatformIO.")
  }

  const view = new DataView(image.buffer, image.byteOffset, image.byteLength)

  const chipId = view.getUint16(ChipIdOffset, true)
  if (chipId !== Esp32S3ChipId) {
    const chipName = chipNames[chipId] ?? `chip ${chipId}`
    throw new FirmwareImageError(`This firmware is built for the ${chipName}, but the modules have an ESP32-S3.`)
  }

  if (view.getUint32(AppDescriptionOffset, true) !== AppDescriptionMagic) {
    throw new FirmwareImageError("This is an ESP32 image, but not an application, e.g. the bootloader. "
      + "Select firmware.bin.")
  }

  if (image[HashAppendedOffset] === 1) {
    if (image.length < ImageHeaderLength + HashLength) {
      throw new FirmwareImageError("The firmware image is incomplete.")
    }

    const content = image.subarray(0, image.length - HashLength)
    const expectedHash = image.subarray(image.length - HashLength)
    const actualHash = new Uint8Array(await crypto.subtle.digest("SHA-256", content))
    if (!actualHash.every((byte, index) => byte === expectedHash[index])) {
      throw new FirmwareImageError("The firmware image is damaged or incomplete, as its checksum doesn't match.")
    }
  }
}
