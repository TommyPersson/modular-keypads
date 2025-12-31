import AssessmentOutlinedIcon from "@mui/icons-material/AssessmentOutlined"

import { Button, CardContent, Popover, Table, TableBody, TableCell, TableHead, TableRow } from "@mui/material"
import { useDeviceFacade } from "@src/modules/device/context"
import type { DeviceMetric } from "@src/modules/device/models"
import { ListDeviceMetricsQuery } from "@src/modules/device/queries/ListDeviceMetricsQuery"
import { useQuery } from "@tanstack/react-query"
import { useCallback, useMemo, useState } from "react"

export const DeviceMetricsDropDown = () => {
  const state = useDeviceMetricsDropDownState()

  return (
    <>
      <Button
        variant={"outlined"}
        color={"inherit"}
        startIcon={<AssessmentOutlinedIcon />}
        onClick={state.handleClick}
        ref={state.setPopoverTargetEl}
      >
        Device Metrics
      </Button>
      <Popover
        id={"test"}
        open={state.isOpen}
        onClose={state.handleClose}
        anchorEl={state.popoverTargetEl}
        sx={{ mt: 1 }}
        anchorOrigin={{
          vertical: "bottom",
          horizontal: "right"
        }}
        transformOrigin={{
          vertical: "top",
          horizontal: "right"
        }}
      >
        <CardContent style={{ maxHeight: "80vh", overflow: "auto", paddingLeft: 0, paddingRight: 0 }}>
          <Table size={"small"}>
            <TableHead>
              <TableRow>
                <TableCell>Metric</TableCell>
                <TableCell>Value</TableCell>
              </TableRow>
            </TableHead>
            <TableBody>
              {state.metrics.map(metric => (
                <TableRow key={metric.name}>
                  <TableCell><code>{metric.name}</code></TableCell>
                  <TableCell align={"right"}><MetricValueText metric={metric} /></TableCell>
                </TableRow>
              ))}
            </TableBody>
          </Table>
        </CardContent>
      </Popover>
    </>
  )
}

function useDeviceMetricsDropDownState() {
  const deviceFacade = useDeviceFacade()

  const metricsQuery = useQuery(ListDeviceMetricsQuery(deviceFacade))
  const metrics = metricsQuery.data ?? []

  const [isOpen, setIsOpen] = useState<boolean>(false)
  const [popoverTargetEl, setPopoverTargetEl] = useState<HTMLElement | null>(null)

  const handleClick = useCallback(() => {
    setIsOpen(true)
  }, [setIsOpen])

  const handleClose = useCallback(() => {
    setIsOpen(false)
  }, [setIsOpen])

  return {
    isOpen,
    popoverTargetEl: popoverTargetEl,
    setPopoverTargetEl,
    metrics,
    handleClick,
    handleClose,
  }
}

const MetricValueText = (props: { metric: DeviceMetric }) => {
  const { metric } = props

  const text = useMemo(() => metric.value.toLocaleString(), [metric.value])

  return (
    <code>{text}</code>
  )
}