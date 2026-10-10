import { useCallback, useMemo, useState } from "react"

const STORAGE_KEY = "overview.deviceOrder"

export type DeviceOrder<T> = {
  orderedDevices: T[]
  moveDevice: (deviceId: string, offset: number) => void
}

/**
 * Orders devices as arranged by the user, as their physical order can't be detected. The order is kept in local
 * storage, including devices that are currently disconnected, so they return to their place when reconnected.
 * Devices that haven't been ordered yet are placed last, in the given order.
 */
export function useDeviceOrder<T extends { deviceId: string }>(devices: T[]): DeviceOrder<T> {
  const [storedOrder, setStoredOrder] = useState<string[]>(loadOrder)

  const orderedDevices = useMemo(() => {
    const indexOf = (device: T) => {
      const index = storedOrder.indexOf(device.deviceId)
      return index >= 0 ? index : Number.MAX_SAFE_INTEGER
    }

    // Stable, so unordered devices keep the given order
    return [...devices].sort((a, b) => indexOf(a) - indexOf(b))
  }, [devices, storedOrder])

  const moveDevice = useCallback((deviceId: string, offset: number) => {
    const fromIndex = orderedDevices.findIndex(it => it.deviceId === deviceId)
    const toIndex = fromIndex + offset
    if (fromIndex < 0 || toIndex < 0 || toIndex >= orderedDevices.length) {
      return
    }

    // Includes the shown devices that weren't ordered yet, in the order they're shown
    const shownIds = orderedDevices.map(it => it.deviceId)
    const newOrder = [...storedOrder, ...shownIds.filter(it => !storedOrder.includes(it))]

    // Swaps the places of the two shown devices, so disconnected devices keep theirs
    const fromOrderIndex = newOrder.indexOf(shownIds[fromIndex])
    const toOrderIndex = newOrder.indexOf(shownIds[toIndex])
    newOrder[fromOrderIndex] = shownIds[toIndex]
    newOrder[toOrderIndex] = shownIds[fromIndex]

    saveOrder(newOrder)
    setStoredOrder(newOrder)
  }, [orderedDevices, storedOrder, setStoredOrder])

  return { orderedDevices, moveDevice }
}

function loadOrder(): string[] {
  try {
    const order: unknown = JSON.parse(localStorage.getItem(STORAGE_KEY) ?? "[]")
    return Array.isArray(order) ? order.filter(it => typeof it === "string") : []
  } catch {
    return []
  }
}

function saveOrder(order: string[]) {
  try {
    localStorage.setItem(STORAGE_KEY, JSON.stringify(order))
  } catch (e) {
    console.warn("Unable to store device order", e)
  }
}
