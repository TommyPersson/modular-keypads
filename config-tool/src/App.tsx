import { DeviceContextProvider } from "./context/DeviceContextProvider"
import { RootScreen } from "./ui/RootScreen"

export const App  = () => {
  return (
    <DeviceContextProvider>
      <RootScreen />
    </DeviceContextProvider>
  )
}


