import { DeviceDebuggerPage } from "@src/modules/device-debugger/ui"
import { DeviceContextProvider } from "@src/modules/device/context"
import { QueryClientProvider } from "@tanstack/react-query"
import { ReactQueryDevtools } from "@tanstack/react-query-devtools"
import { createBrowserRouter, Navigate, RouterProvider } from "react-router"
import { RootScreen } from "./modules/root/ui"
import { queryClient } from "./utils/queryClient"

const router = createBrowserRouter([
  {
    path: "/",
    Component: RootScreen,
    children: [
      {
        index: true,
        element: <Navigate to="/device-debugger" replace />
      },
      {
        path: "device-debugger",
        Component: DeviceDebuggerPage,
      }
    ]
  }
])

export const App = () => {
  return (
    <QueryClientProvider client={queryClient}>
      <ReactQueryDevtools />
      <DeviceContextProvider>
        <RouterProvider router={router} />
      </DeviceContextProvider>
    </QueryClientProvider>
  )
}


