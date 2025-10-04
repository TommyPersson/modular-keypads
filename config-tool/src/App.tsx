import { QueryClientProvider } from "@tanstack/react-query"
import { DeviceContextProvider } from "./context/DeviceContextProvider"
import { RootScreen } from "./ui/RootScreen"
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


