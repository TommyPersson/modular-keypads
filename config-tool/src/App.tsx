import { DeviceContextProvider } from "@src/modules/device/context"
import { QueryClientProvider } from "@tanstack/react-query"
import { RootScreen } from "./modules/root/ui"
import { queryClient } from "./utils/queryClient"


export const App = () => {
  return (
    <QueryClientProvider client={queryClient}>
      <DeviceContextProvider>
        <RootScreen />
      </DeviceContextProvider>
    </QueryClientProvider>
  )
}


