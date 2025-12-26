import { useEffect, useState } from "react"
import { defaultDeviceContextValue, DeviceContext, globalDeviceFacade } from "./DeviceContext"

export const DeviceContextProvider = (props: { children: any }) => {
  const [value, setValue] = useState(defaultDeviceContextValue)

  const facade = value.facade

  useEffect(() => {
    const subscription = facade.isConnected$.subscribe(next => {
      setValue(s=> ({ ...s, isConnected: next }))
    })

    globalDeviceFacade.value = facade

    return () => subscription.unsubscribe()
  }, [facade, setValue])

  return (
    <DeviceContext.Provider value={value} {...props} />
  )
}