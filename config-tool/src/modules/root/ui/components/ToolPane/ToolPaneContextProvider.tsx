import { ToolPaneContext } from "@src/modules/root/ui/components/ToolPane/ToolPaneContext"
import { useCallback, useState } from "react"

export const ToolPaneContextProvider = (props: {
  children: any
  // The tool pane that is open at first, e.g. in stories
  initialSelectedToolId?: string | null
}) => {
  const [selectedToolId, setSelectedToolId] = useState<string | null>(props.initialSelectedToolId ?? null)

  const handleSelectedToolIdChange = useCallback((value: string | null) => {
    setSelectedToolId(s => {
      if (value === s) {
        return null
      } else {
        return value
      }
    })

  }, [setSelectedToolId])

  const value = { selectedToolId, setSelectedToolId: handleSelectedToolIdChange }

  return (
    <ToolPaneContext.Provider value={value}>
      {props.children}
    </ToolPaneContext.Provider>
  )
}