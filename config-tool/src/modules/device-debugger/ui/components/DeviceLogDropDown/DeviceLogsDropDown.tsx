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
  Popover,
  Stack,
  ToggleButton,
  ToggleButtonGroup,
  Typography
} from "@mui/material"
import { useDeviceFacade } from "@src/modules/device/context"
import type { RawLogMessage } from "@src/modules/device/facade"
import { takeFirst } from "@src/utils/arrays"
import type { DateTime } from "luxon"
import { type ComponentProps, forwardRef, memo, useCallback, useEffect, useMemo, useState } from "react"
import { TransitionGroup } from "react-transition-group"

export const DeviceLogsDropDown = () => {
  const state = useDeviceLogsDropDownState()

  return (
    <>
      <Badge badgeContent={state.filteredLogMessages.length} color={"secondary"}>
        <Button
          variant={"outlined"}
          color={"inherit"}
          startIcon={<WysiwygOutlinedIcon />}
          onClick={state.handleClick}
          ref={state.setPopoverTargetEl}
        >
          Device Logs
        </Button>
      </Badge>
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
        <CardHeader
          title={"Log Messages"}
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
      </Popover>
    </>
  )
}

let nextMessageKey = 0

const initialLevels = ["info", "warning", "error"]

function useDeviceLogsDropDownState() {
  const deviceFacade = useDeviceFacade()

  const [logMessages, setLogMessages] = useState<ParsedLogMessage[]>([])
  const [logLevels, setLogLevels] = useState<string[]>(initialLevels)

  const filteredLogMessages = useMemo(() => {
    return takeFirst(logMessages.filter(it => logLevels.includes(it.level)), 50)
  }, [logMessages, logLevels])

  useEffect(() => {
    const subscription = deviceFacade.logs$.subscribe(logMessage => {
      if (logMessage.direction !== "to-host" || !logMessage.message.startsWith("#")) {
        return
      }
      nextMessageKey++
      setLogMessages(s => {
        const next = [parseLogMessage(logMessage, nextMessageKey), ...s]
        return takeFirst(next, 1000)
      })
    })
    return () => subscription.unsubscribe()
  }, [deviceFacade, setLogMessages])

  const [isOpen, setIsOpen] = useState<boolean>(false)
  const [popoverTargetEl, setPopoverTargetEl] = useState<HTMLElement | null>(null)

  const handleClick = useCallback(() => {
    setIsOpen(true)
  }, [setIsOpen])

  const handleClose = useCallback(() => {
    setIsOpen(false)
  }, [setIsOpen])

  const handleLogLevelsChange = useCallback((_: any, values: string[]) => {
    setLogLevels(values)
  }, [setLogLevels])

  const handleClearClick = useCallback(() => {
    setLogMessages([])
  }, [setLogMessages])

  const handleLogMessageClosed = useCallback((logMessage: ParsedLogMessage) => {
    setLogMessages(s => s.filter(it => it !== logMessage))
  }, [setLogMessages])

  return {
    logMessages,
    filteredLogMessages,
    isOpen,
    logLevels,
    popoverTargetEl: popoverTargetEl,
    setPopoverTargetEl,
    handleClick,
    handleClose,
    handleLogLevelsChange,
    handleClearClick,
    handleLogMessageClosed,
  }
}

type ParsedLogMessage = {
  key: number
  timestamp: DateTime
  level: string
  component: string | null
  message: string
}

function parseLogMessage(rawMessage: RawLogMessage, key: number): ParsedLogMessage {
  const [level, component, ...messageParts] = rawMessage.message.substring(1).split(":")
  const message = messageParts.length > 0 ? messageParts.join(":") : null

  return {
    key: key,
    timestamp: rawMessage.timestamp,
    level,
    component: message ? component : null,
    message: message ? message : component
  }
}

const LogMessageAlert = memo(forwardRef((props: {
  logMessage: ParsedLogMessage
  onClose: (logMessage: ParsedLogMessage) => void
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

function getSeverity(logMessage: ParsedLogMessage): ComponentProps<typeof Alert>["severity"] {
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

function getIcon(logMessage: ParsedLogMessage): ComponentProps<typeof Alert>["icon"] {
  switch (logMessage.level) {
    case "debug":
      return <BugReportOutlinedIcon />
    default:
      return undefined
  }
}