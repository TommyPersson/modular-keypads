import { AllAppModules } from "@src/AllAppModules"
import { ThemeProvider } from "@mui/material"
import { ConfirmProvider } from "@src/modules/common/components"
import { DeviceContextProvider } from "@src/modules/device/context"
import { ToolPaneContextProvider } from "@src/modules/root/ui/components/ToolPane/ToolPaneContextProvider"
import { QueryClientProvider } from "@tanstack/react-query"
import { ReactQueryDevtools } from "@tanstack/react-query-devtools"
import { createBrowserRouter, Navigate, RouterProvider } from "react-router"
import { RootScreen } from "./modules/root/ui"
import { theme } from "./modules/root/ui/theme"
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
    <ThemeProvider theme={theme}>
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
    </ThemeProvider>
  )
}


