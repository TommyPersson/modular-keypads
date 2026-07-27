import { NavItem } from "@src/modules/root/ui"
import type { ToolPaneDefinition } from "@src/modules/root/ui/components/ToolPane/ToolPaneDefinition"
import * as React from "react"
import type { RouteObject } from "react-router"

export type AppModule = {
  id: string
  order: number
  routes: RouteObject[]
  navItems: React.ComponentProps<typeof NavItem>[],
  toolPanes: ToolPaneDefinition[]
}