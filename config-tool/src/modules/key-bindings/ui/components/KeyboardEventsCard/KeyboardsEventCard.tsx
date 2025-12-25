import FileDownloadIcon from "@mui/icons-material/FileDownload"
import FileUploadIcon from "@mui/icons-material/FileUpload"

import {
  Alert,
  Avatar,
  Button,
  Card,
  CardContent,
  CardHeader,
  Divider,
  Stack,
  Table,
  TableBody,
  TableCell,
  TableHead,
  TableRow,
  ToggleButton,
  Tooltip
} from "@mui/material"
import { EmptyTableRow } from "@src/modules/common/components"
import { keyboadKeyCodes } from "@src/modules/key-bindings/data"
import { DateTime } from "luxon"
import { memo, useCallback, useEffect, useMemo, useState } from "react"

const maxEventLogItems = 20

export const KeyboardEventsCard = () => {
  const [enabled, setEnabled] = useState(true)
  const [keyCaptureEnabled, setKeyCaptureEnabled] = useState(false)

  const keyboardState = useKeyboardState(enabled, keyCaptureEnabled)

  const handleKeyCaptureToggled = useCallback(() => {
    setKeyCaptureEnabled(s => !s)
  }, [setKeyCaptureEnabled])

  const isAnyModalOpen = document.getElementsByClassName("MuiDialog-root").length > 0
  useEffect(() => {
    if (isAnyModalOpen) {
      setEnabled(false)
    } else {
      setEnabled(true)
    }
  }, [isAnyModalOpen, setEnabled])

  return (
    <Card>
      <CardHeader
        title={"Keyboard Events"}
        subheader={"View live keyboard events to help create or debug your macros"}
        action={
          <>
            <ToggleButton
              onClick={handleKeyCaptureToggled}
              selected={keyCaptureEnabled}
              value={"dont-care"}
            >
              Key Capture
            </ToggleButton>
            <Button onClick={keyboardState.reset}>Reset</Button>
          </>
        }
      />
      <CardContent>
        <Alert severity={"info"}>
          It is possible for this view to get out of sync with the keyboard if the pressed combination shifts focus
          away from the window, or trigger some other behavior. In that case you may reset the state of the preview.
          <p />
          You can also use the "Key Capture" button to prevent the browser from handling most of the pressed keys.
        </Alert>
      </CardContent>
      <Divider />
      <KeyboardStateCardContent state={keyboardState} />
      <Divider />
      <KeyboardLogCardContent state={keyboardState} />
    </Card>
  )
}

const KeyboardStateCardContent = memo((props: {
  state: KeyboardState
}) => {
  const { state } = props
  return (
    <>
      <CardHeader title={"Currently Pressed Keys"} />
      <CardContent style={{ paddingLeft: 0, paddingRight: 0 }}>
        <Stack spacing={2} style={{ height: 200, overflow: "auto" }}>
          <Table size={"small"}>
            <TableHead>
              <TableRow>
                <TableCell style={{ width: 0, whiteSpace: "nowrap" }}>
                  HID Code
                </TableCell>
                <TableCell style={{ whiteSpace: "nowrap" }}>
                  HID Description
                </TableCell>
                <TableCell style={{ width: 150, whiteSpace: "nowrap" }}>
                  JS Code
                </TableCell>
                <TableCell style={{ width: 200, whiteSpace: "nowrap" }} align={"right"}>
                  Category
                </TableCell>
              </TableRow>
            </TableHead>
            <TableBody>
              {state.pressedKeyCodes.length === 0 && (
                <EmptyTableRow>
                  There are currently no keys pressed.
                </EmptyTableRow>
              )}
              {state.pressedKeyCodes.map(jsCode => (
                <PressedKeyRow key={jsCode} jsCode={jsCode} />
              ))}
            </TableBody>
          </Table>
          <div style={{ flex: 1 }} />
        </Stack>
      </CardContent>
    </>
  )
})

const PressedKeyRow = memo((props: {
  jsCode: string
}) => {
  const { jsCode } = props
  const keyCode = keyboadKeyCodes.byJsCode[jsCode] ?? null

  return (
    <TableRow hover>
      <TableCell>
        <code>0x{keyCode?.hidCode?.toString(16)?.padStart(2, "0") ?? "N/A"}</code>
      </TableCell>
      <TableCell>
        {keyCode?.hidDescription ?? "N/A"}<br /><em>{keyCode?.note}</em>
      </TableCell>
      <TableCell>
        <code>{keyCode?.jsCode ?? jsCode}</code>
      </TableCell>
      <TableCell style={{ whiteSpace: "nowrap" }} align={"right"}>
        {keyCode?.category ?? "Other"}
      </TableCell>
    </TableRow>
  )
})

const KeyboardLogCardContent = memo((props: {
  state: KeyboardState
}) => {
  const { state } = props
  const logItems = useMemo(() => state.eventLog.toReversed(), [state.eventLog])
  return (
    <>
      <CardHeader
        title={"Event Log"}
        subheader={`The last ${maxEventLogItems} events are shown. The most recent events are at the top.`}
      />
      <CardContent style={{ paddingLeft: 0, paddingRight: 0 }}>
        <Stack spacing={2} style={{ height: 400, overflow: "auto" }}>
          <Table size={"small"}>
            <TableHead>
              <TableRow>
                <TableCell style={{ width: 100, whiteSpace: "nowrap" }}>
                  Time
                </TableCell>
                <TableCell style={{ width: 0, whiteSpace: "nowrap" }}>
                  Event
                </TableCell>
                <TableCell style={{ width: 0, whiteSpace: "nowrap" }}>
                  HID Code
                </TableCell>
                <TableCell style={{ whiteSpace: "nowrap" }}>
                  HID Description
                </TableCell>
                <TableCell style={{ width: 150, whiteSpace: "nowrap" }}>
                  JS Code
                </TableCell>
                <TableCell style={{ width: 200, whiteSpace: "nowrap" }} align={"right"}>
                  Category
                </TableCell>
              </TableRow>
            </TableHead>
            <TableBody>
              {logItems.length === 0 && (
                <EmptyTableRow>No events recorded yet.</EmptyTableRow>
              )}
              {logItems.map(logItem =>
                <LogItemRow key={logItem.type + logItem.timestamp.toMillis()} logItem={logItem} />
              )}
            </TableBody>
          </Table>
          <div style={{ flex: 1 }} />
        </Stack>
      </CardContent>
    </>
  )
})

const LogItemRow = memo((props: {
  logItem: KeyboardEventLogItem
}) => {
  const { logItem } = props

  const jsCode = logItem.jsCode
  const keyCode = keyboadKeyCodes.byJsCode[jsCode] ?? null

  return (
    <TableRow hover>
      <TableCell>
        {logItem.timestamp.toFormat("HH:mm:ss")}
        .
        <strong>
          {logItem.timestamp.toFormat("SSS")}
        </strong>
      </TableCell>
      <TableCell style={{ whiteSpace: "nowrap" }}>
        <center>
          <Tooltip title={logItem.type}>
            <Avatar
              sx={{
                width: 22,
                height: 22,
                bgcolor: logItem.type === "Pressed"
                  ? "primary.main"//blue["A400"]
                  : "warning.main" //orange["A400"]
              }}
              variant={"circular"}
            >
              {logItem.type === "Pressed"
                ? <FileDownloadIcon fontSize={"small"} />
                : <FileUploadIcon fontSize={"small"} />}
            </Avatar>
          </Tooltip>
        </center>
      </TableCell>
      <TableCell>
        <code>0x{keyCode?.hidCode?.toString(16)?.padStart(2, "0") ?? "N/A"}</code>
      </TableCell>
      <TableCell>
        {keyCode?.hidDescription ?? "N/A"}<br /><em>{keyCode?.note}</em>
      </TableCell>
      <TableCell>
        <code>{keyCode?.jsCode ?? jsCode}</code>
      </TableCell>
      <TableCell style={{ whiteSpace: "nowrap" }} align={"right"}>
        {keyCode?.category ?? "Other"}
      </TableCell>
    </TableRow>
  )
})

type KeyboardState = {
  pressedKeyCodes: string[]
  eventLog: KeyboardEventLogItem[]
  reset(): void
}

type KeyboardEventLogItem = {
  timestamp: DateTime,
  type: "Pressed" | "Released",
  jsCode: string
}

function useKeyboardState(enabled: boolean, keyCaptureEnabled: boolean): KeyboardState {
  const [pressedKeyCodes, setPressedKeyCodes] = useState<string[]>([])
  const [eventLog, setEventLog] = useState<KeyboardEventLogItem[]>([])

  const handleReset = useCallback(() => {
    setPressedKeyCodes([])
    setEventLog([])
  }, [setPressedKeyCodes, setEventLog])

  useEffect(() => {
    const keyDownHandler = (e: KeyboardEvent) => {
      if (e.code.length === 0) {
        return
      }

      if (!e.repeat) {
        setPressedKeyCodes(s => {
          if (!s.includes(e.code)) {
            return [...s, e.code]
          } else {
            return s
          }
        })

        setEventLog(s => {
          const logItem = { timestamp: DateTime.now(), type: "Pressed", jsCode: e.code } satisfies KeyboardEventLogItem
          const next = [...s, logItem]
          return takeLast(next, maxEventLogItems)
        })
      }

      if (keyCaptureEnabled) {
        e.preventDefault()
        e.stopPropagation()
        e.stopImmediatePropagation()
      }
    }

    const keyUpHandler = (e: KeyboardEvent) => {
      if (e.code.length === 0) {
        return
      }

      setPressedKeyCodes(s => s.filter(it => it !== e.code))
      setEventLog(s => {
        const logItem = { timestamp: DateTime.now(), type: "Released", jsCode: e.code } satisfies KeyboardEventLogItem
        const next = [...s, logItem]
        return takeLast(next, maxEventLogItems)
      })
    }

    if (enabled) {
      document.addEventListener("keydown", keyDownHandler)
      document.addEventListener("keyup", keyUpHandler)
    }

    return () => {
      document.removeEventListener("keydown", keyDownHandler)
      document.removeEventListener("keyup", keyUpHandler)
    }
  }, [enabled, keyCaptureEnabled, setPressedKeyCodes, setEventLog])

  useEffect(() => {
    handleReset()
  }, [enabled, handleReset])

  return {
    pressedKeyCodes,
    eventLog,
    reset: handleReset
  }
}

function takeLast<T>(arr: T[], count: number): T[] {
  return arr.length > count ? arr.slice(arr.length - count) : arr
}