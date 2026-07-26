import * as React from "react"
import { useContext } from "react"

export type ToolPaneContextValue = {
  selectedToolId: string | null
  setSelectedToolId: (value: string | null) => void
}

export const defaultToolPaneContextValue: ToolPaneContextValue = {
  selectedToolId: null,
  setSelectedToolId: () => {
  }
}

export const ToolPaneContext = React.createContext(defaultToolPaneContextValue)

export function useToolPaneContext(): ToolPaneContextValue {
  return useContext(ToolPaneContext)
}
