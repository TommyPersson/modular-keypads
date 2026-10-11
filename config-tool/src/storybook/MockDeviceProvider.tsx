import { DeviceContext, globalDeviceFacade } from "@src/modules/device/context"
import { QueryClient, QueryClientProvider } from "@tanstack/react-query"
import { type ReactNode, useEffect, useState } from "react"
import { MockDeviceFacade, type MockDeviceOptions } from "./MockDeviceFacade"

/**
 * Provides a mock device, and a query client of its own so that cached device data doesn't leak between stories.
 *
 * The mock device also replaces the global device facade, which the commands use, while the provider is mounted. As
 * the commands refresh the queries of the app's query client, the mock device refreshes the queries of this one.
 */
export const MockDeviceProvider = (props: {
  options: MockDeviceOptions
  children: ReactNode
}) => {
  const { options, children } = props

  const [facade] = useState(() => new MockDeviceFacade(options))
  const [isConnected, setIsConnected] = useState(facade.isConnected)
  const [queryClient] = useState(() => new QueryClient({
    defaultOptions: { queries: { retry: false, staleTime: Infinity } },
  }))

  useEffect(() => {
    const previousFacade = globalDeviceFacade.value
    globalDeviceFacade.value = facade
    facade.onQueryDataChanged = queryKey => queryClient.invalidateQueries({ queryKey })
    facade.start()
    const subscription = facade.isConnected$.subscribe(setIsConnected)

    return () => {
      subscription.unsubscribe()
      facade.stop()
      facade.onQueryDataChanged = () => {}
      globalDeviceFacade.value = previousFacade
    }
  }, [facade, queryClient])

  return (
    <QueryClientProvider client={queryClient}>
      <DeviceContext.Provider value={{ facade, isConnected }}>
        {children}
      </DeviceContext.Provider>
    </QueryClientProvider>
  )
}
