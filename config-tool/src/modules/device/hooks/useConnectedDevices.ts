import { useDeviceFacade } from "@src/modules/device/context"
import type { DeviceInformation, NotificationMessage } from "@src/modules/device/models"
import { ListConnectedDevicesQuery } from "@src/modules/device/queries/ListConnectedDevicesQuery"
import { useQuery, useQueryClient, type UseQueryResult } from "@tanstack/react-query"
import { useEffect } from "react"
import { debounceTime, filter } from "rxjs"

const DEVICE_LIST_CHANGE_TYPES = new Set(["device.connected", "device.disconnected", "device.renamed"])

// Changes often come in bursts, e.g. from a scan finding several devices, which are then listed only once
const DEVICE_LIST_REFRESH_DEBOUNCE_MS = 250

export function useConnectedDevices(): UseQueryResult<DeviceInformation[]> {
  const deviceFacade = useDeviceFacade()
  const queryClient = useQueryClient()
  const queryOptions = ListConnectedDevicesQuery(deviceFacade)
  const connectedDevicesQuery = useQuery(queryOptions)

  const queryKey = queryOptions.queryKey

  useEffect(() => {
    const subscription = deviceFacade.notifications$
      .pipe(
        filter((message: NotificationMessage) => DEVICE_LIST_CHANGE_TYPES.has(message.type)),
        debounceTime(DEVICE_LIST_REFRESH_DEBOUNCE_MS),
      )
      .subscribe(() => {
        queryClient.invalidateQueries({ queryKey }).then()
      })
    return () => subscription.unsubscribe()
    // eslint-disable-next-line
  }, [deviceFacade, queryClient])

  return connectedDevicesQuery
}
