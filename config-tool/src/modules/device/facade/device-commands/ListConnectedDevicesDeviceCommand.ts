import type { DeviceInformation } from "@src/modules/device/models"
import { DeviceCommand } from "./DeviceCommand"

export class ListConnectedDevicesDeviceCommand extends DeviceCommand<DeviceInformation[]> {
  override type: string = "list.connected.devices"

  override parseResponse(lines: string[]): DeviceInformation[] {

    return lines.map(line => {
      const [idHex, addressHex, type, name, parentIdHex, slotText] = line.split(",")
      const slot = parseInt(slotText)
      return {
        deviceId: idHex,
        deviceFirmwareVersion: "0.0.1", // TODO figure out something to do with versioning
        deviceType: type,
        deviceAddress: parseInt(addressHex, 16),
        deviceName: name,
        deviceRegisterNames: [],
        parentDeviceId: parentIdHex && !/^0+$/.test(parentIdHex) ? parentIdHex : null,
        subDeviceSlot: slot > 0 ? slot : null,
      } satisfies DeviceInformation
    })
  }
}