import { AllAppModules } from "@src/AllAppModules"
import { ConfirmProvider } from "@src/modules/common/components"
import { DeviceContextProvider } from "@src/modules/device/context"
import { ToolPaneContextProvider } from "@src/modules/root/ui/components/ToolPane/ToolPaneContextProvider"
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
        element: <Navigate to="/overview" replace />
      },
      ...AllAppModules.flatMap(it => it.routes),
    ]
  }
])

export const App = () => {
  return (
    <QueryClientProvider client={queryClient}>
      <ReactQueryDevtools />
      <DeviceContextProvider>
        <ConfirmProvider>
          <ToolPaneContextProvider>
            <RouterProvider router={router} />
          </ToolPaneContextProvider>
        </ConfirmProvider>
      </DeviceContextProvider>
    </QueryClientProvider>
  )
}


