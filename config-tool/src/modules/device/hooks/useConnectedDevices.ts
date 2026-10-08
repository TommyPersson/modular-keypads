import { useDeviceFacade } from "@src/modules/device/context"
import type { DeviceInformation, NotificationMessage } from "@src/modules/device/models"
import { ListConnectedDevicesQuery } from "@src/modules/device/queries/ListConnectedDevicesQuery"
import { useQuery, useQueryClient, type UseQueryResult } from "@tanstack/react-query"
import { useEffect } from "react"

export function useConnectedDevices(): UseQueryResult<DeviceInformation[]> {
  const deviceFacade = useDeviceFacade()
  const queryClient = useQueryClient()
  const queryOptions = ListConnectedDevicesQuery(deviceFacade)
  const connectedDevicesQuery = useQuery(queryOptions)

  const queryKey = queryOptions.queryKey

  useEffect(() => {
    const subscription = deviceFacade.notifications$.subscribe((message: NotificationMessage) => {
      if (message.type === "device.connected" || message.type === "device.disconnected") {
        queryClient.invalidateQueries({ queryKey }).then()
      }
    })
    return () => subscription.unsubscribe()
    // eslint-disable-next-line
  }, [deviceFacade, queryClient])

  return connectedDevicesQuery
}
