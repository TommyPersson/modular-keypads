import { useDeviceFacade } from "@src/modules/device/context"
import type { DeviceInformation } from "@src/modules/device/models"
import { GetDeviceInformationQuery } from "@src/modules/device/queries"
import { useQuery } from "@tanstack/react-query"

export function useDeviceInformation(): [DeviceInformation | null, () => void] {
  const deviceFacade = useDeviceFacade()
  const deviceInformationQuery = useQuery(GetDeviceInformationQuery(deviceFacade))

  return [
    deviceInformationQuery.data ?? null,
    deviceInformationQuery.refetch
  ]
}