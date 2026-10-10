import { DeviceContext } from "@src/modules/device/context"
import { QueryClient, QueryClientProvider } from "@tanstack/react-query"
import { type ReactNode, useEffect, useState } from "react"
import { MockDeviceFacade, type MockDeviceOptions } from "./MockDeviceFacade"

/**
 * Provides a mock device, and a query client of its own so that cached device data doesn't leak between stories.
 */
export const MockDeviceProvider = (props: {
  options: MockDeviceOptions
  children: ReactNode
}) => {
  const { options, children } = props

  const [facade] = useState(() => new MockDeviceFacade(options))
  const [queryClient] = useState(() => new QueryClient({
    defaultOptions: { queries: { retry: false, staleTime: Infinity } },
  }))

  useEffect(() => {
    facade.start()
    return () => facade.stop()
  }, [facade])

  return (
    <QueryClientProvider client={queryClient}>
      <DeviceContext.Provider value={{ facade, isConnected: true }}>
        {children}
      </DeviceContext.Provider>
    </QueryClientProvider>
  )
}
