import BugReportOutlinedIcon from "@mui/icons-material/BugReportOutlined"
import DeleteSweepOutlinedIcon from "@mui/icons-material/DeleteSweepOutlined"
import WysiwygOutlinedIcon from "@mui/icons-material/WysiwygOutlined"

import {
  Alert,
  Badge,
  Box,
  Button,
  CardContent,
  CardHeader,
  Collapse,
  Stack,
  ToggleButton,
  ToggleButtonGroup,
  Typography
} from "@mui/material"
import { GetDeviceLogsQuery } from "@src/modules/device-debugger/queries"
import { useDeviceFacade } from "@src/modules/device/context"
import type { LogMessage } from "@src/modules/device/models"
import { takeFirst } from "@src/utils/arrays"
import { useQuery } from "@tanstack/react-query"
import { type ComponentProps, forwardRef, memo, useCallback, useMemo, useState } from "react"
import { TransitionGroup } from "react-transition-group"

export const DeviceLogsToolPaneIcon = () => {
  const state = useDeviceLogsToolPaneState()

  return (
    <Badge badgeContent={state.filteredLogMessages.length} color={"secondary"}>
      <WysiwygOutlinedIcon />
    </Badge>
  )
}

export const DeviceLogsToolPaneContent = () => {
  const state = useDeviceLogsToolPaneState()

  return (
    <Stack>
      <CardHeader
        title={
          <Stack spacing={1} direction={"row"} alignItems={"center"}>
            <WysiwygOutlinedIcon />
            <span>Log Messages</span>
          </Stack>
        }
        action={(
          <Stack direction={"row"} gap={2}>
            <ToggleButtonGroup color={"primary"} value={state.logLevels} onChange={state.handleLogLevelsChange}>
              <ToggleButton value={"debug"}>Debug</ToggleButton>
              <ToggleButton value={"info"}>Info</ToggleButton>
              <ToggleButton value={"warning"}>Warning</ToggleButton>
              <ToggleButton value={"error"}>Error</ToggleButton>
            </ToggleButtonGroup>
            <Button
              startIcon={<DeleteSweepOutlinedIcon />}
              onClick={state.handleClearClick}
              children={"Clear"}
            />
          </Stack>
        )}
      />
      <CardContent style={{ maxHeight: "80vh", overflow: "auto" }}>
        {state.filteredLogMessages.length === 0 && (
          state.logMessages.length > 0 ? (
            <center><em>There are no messages matching the current filter.</em></center>
          ) : (
            <center><em>No log messages have been received, yet.</em></center>
          )
        )}
        <TransitionGroup style={{ display: "flex", flexDirection: "column", gap: 8, width: 600, minHeight: 400 }}>
          {state.filteredLogMessages.map(logMessage => {
            return (
              <Collapse>
                <LogMessageAlert
                  key={logMessage.key}
                  logMessage={logMessage}
                  onClose={state.handleLogMessageClosed}
                />
              </Collapse>
            )
          })}
        </TransitionGroup>
        {state.filteredLogMessages.length > 0 && state.filteredLogMessages.length < state.logMessages.length && (
          <center style={{ marginTop: 8 }}>
            <em>
              Only showing <strong>{state.filteredLogMessages.length}</strong> out
              of <strong>{state.logMessages.length}</strong> items.
            </em>
          </center>
        )}
      </CardContent>
    </Stack>
  )
}

const initialLevels = ["info", "warning", "error"]

const EmptyArray: any[] = []

function useDeviceLogsToolPaneState() {
  const deviceFacade = useDeviceFacade()

  const logMessagesQuery = useQuery(GetDeviceLogsQuery(deviceFacade))
  const logMessages: LogMessage[] = logMessagesQuery.data ?? EmptyArray

  const [logLevels, setLogLevels] = useState<string[]>(initialLevels)

  const filteredLogMessages = useMemo(() => {
    return takeFirst(logMessages.filter(it => logLevels.includes(it.level)), 50)
  }, [logMessages, logLevels])

  const handleLogLevelsChange = useCallback((_: any, values: string[]) => {
    setLogLevels(values)
  }, [setLogLevels])

  const handleClearClick = useCallback(() => {
    deviceFacade.clearLogs()
  }, [deviceFacade])

  const handleLogMessageClosed = useCallback((logMessage: LogMessage) => {
    deviceFacade.deleteLogMessage(logMessage.key)
  }, [deviceFacade])

  return {
    logMessages,
    filteredLogMessages,
    logLevels,
    handleLogLevelsChange,
    handleClearClick,
    handleLogMessageClosed,
  }
}

const LogMessageAlert = memo(forwardRef((props: {
  logMessage: LogMessage
  onClose: (logMessage: LogMessage) => void
}, ref) => {
  const { logMessage, onClose } = props
  const { timestamp, component, message } = logMessage

  const severity = getSeverity(logMessage)
  const icon = getIcon(logMessage)

  const handleClose = useCallback(() => {
    onClose(logMessage)
  }, [logMessage, onClose])


  return (
    <Alert
      severity={severity}
      icon={icon}
      onClose={handleClose}
      ref={ref as any}
      slotProps={{
        message: { style: { width: "100%" } }
      }}>
      <Stack direction={"row"} style={{ width: "100%" }}>
        <Box flex={1}>
          {message}
        </Box>
        <Stack alignItems={"flex-end"} gap={1}>
          <Typography variant={"caption"}>
            at {timestamp.toFormat("HH:mm:ss")}
            .
            <strong>{timestamp.toFormat("SSS")}</strong>
          </Typography>
          <Typography variant={"caption"}>
            from <code>{component}</code>
          </Typography>
        </Stack>
      </Stack>
    </Alert>
  )
}))

function getSeverity(logMessage: LogMessage): ComponentProps<typeof Alert>["severity"] {
  switch (logMessage.level) {
    case "debug":
      return "info"
    case "info":
      return "info"
    case "error":
      return "error"
    case "warning":
      return "warning"
  }
}

function getIcon(logMessage: LogMessage): ComponentProps<typeof Alert>["icon"] {
  switch (logMessage.level) {
    case "debug":
      return <BugReportOutlinedIcon />
    default:
      return undefined
  }
}