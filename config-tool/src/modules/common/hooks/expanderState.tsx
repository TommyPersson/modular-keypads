import ExpandMoreOutlinedIcon from "@mui/icons-material/ExpandMoreOutlined"
import ExpandLessOutlinedIcon from "@mui/icons-material/ExpandLessOutlined"

import { useCallback, useState } from "react"
import * as React from "react"

export type ExpanderState = {
  isExpanded: boolean
  icon: React.ReactElement
  toggle: () => void
}

export function useExpanderState(defaultValue: boolean): ExpanderState {
  const [isExpanded, setIsExpanded] = useState(defaultValue)

  const toggle = useCallback(() => setIsExpanded(s => !s), [setIsExpanded])

  const icon = isExpanded ? <ExpandMoreOutlinedIcon /> : <ExpandLessOutlinedIcon />

  return {
    isExpanded,
    icon,
    toggle
  }
}