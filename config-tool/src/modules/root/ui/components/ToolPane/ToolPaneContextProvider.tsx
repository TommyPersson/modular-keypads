import { ToolPaneContext } from "@src/modules/root/ui/components/ToolPane/ToolPaneContext"
import { useCallback, useState } from "react"

export const ToolPaneContextProvider = (props: {
  children: any
}) => {
  const [selectedToolId, setSelectedToolId] = useState<string | null>(null)

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