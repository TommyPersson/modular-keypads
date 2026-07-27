import AssessmentOutlinedIcon from "@mui/icons-material/AssessmentOutlined"

import { CardContent, CardHeader, Stack, Table, TableBody, TableCell, TableHead, TableRow } from "@mui/material"
import { useDeviceFacade } from "@src/modules/device/context"
import type { DeviceMetric } from "@src/modules/device/models"
import { ListDeviceMetricsQuery } from "@src/modules/device/queries/ListDeviceMetricsQuery"
import { useQuery } from "@tanstack/react-query"
import { useCallback, useMemo, useState } from "react"

export const DeviceMetricsToolPaneContent = () => {
  const state = useDeviceMetricsDropDownState()

  return (
    <>
      <CardHeader
        title={
          <Stack spacing={1} direction={"row"} alignItems={"center"}>
            <AssessmentOutlinedIcon />
            <span>Device Metrics</span>
          </Stack>
        }
      />
      <CardContent style={{ maxHeight: "80vh", overflow: "auto", paddingLeft: 0, paddingRight: 0 }}>
        <Table size={"small"}>
          <TableHead>
            <TableRow>
              <TableCell>Metric</TableCell>
              <TableCell align={"right"}>Value</TableCell>
            </TableRow>
          </TableHead>
          <TableBody>
            {state.metrics.map(metric => (
              <TableRow key={metric.name} hover>
                <TableCell><code>{metric.name}</code></TableCell>
                <TableCell align={"right"}><MetricValueText metric={metric} /></TableCell>
              </TableRow>
            ))}
            {state.metrics.length === 0 && (
              <TableRow>
                <TableCell colSpan={2} align={"center"}><em>No metrics available</em></TableCell>
              </TableRow>
            )}
          </TableBody>
        </Table>
      </CardContent>
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