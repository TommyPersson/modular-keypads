import type { ToolPaneDefinition } from "@src/modules/root/ui/components/ToolPane/ToolPaneDefinition"
import type { RouteObject } from "react-router"

export type AppModule = {
  id: string
  order: number
  routes: RouteObject[]
  toolPanes: ToolPaneDefinition[]
}