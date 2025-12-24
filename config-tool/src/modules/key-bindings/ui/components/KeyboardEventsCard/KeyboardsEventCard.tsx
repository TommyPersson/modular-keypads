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
  Tooltip, Typography
} from "@mui/material"
import { blue, deepPurple } from "@mui/material/colors"
import { useDeviceFacade } from "@src/modules/device/context"
import { keyboadKeyCodes } from "@src/modules/key-bindings/data"
import { DateTime } from "luxon"
import { useCallback, useEffect, useMemo, useState } from "react"

const maxEventLogItems = 20

export const KeyboardEventsCard = () => {
  const [keyCaptureEnabled, setKeyCaptureEnabled] = useState(false)

  const keyboardState = useKeyboardState(keyCaptureEnabled)

  const handleKeyCaptureToggled = useCallback(() => {
    setKeyCaptureEnabled(s => !s)
  }, [setKeyCaptureEnabled])

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

const KeyboardStateCardContent = (props: {
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
                <TableCell style={{ width: 0, whiteSpace: "nowrap" }}>HID Code</TableCell>
                <TableCell style={{ whiteSpace: "nowrap" }}>HID Description</TableCell>
                <TableCell style={{ width: 150, whiteSpace: "nowrap" }}>JS Code</TableCell>
                <TableCell style={{ width: 200, whiteSpace: "nowrap" }} align={"right"}>Category</TableCell>
              </TableRow>
            </TableHead>
            <TableBody>
              {state.pressedKeyCodes.length === 0 && (
                <TableRow>
                  <TableCell colSpan={6}>
                    <center>
                      <em>There are currently no keys pressed.</em>
                    </center>
                  </TableCell>
                </TableRow>
              )}
              {state.pressedKeyCodes.map(jsCode => {
                const keyCode = keyboadKeyCodes.byJsCode[jsCode] ?? null
                return (
                  <TableRow key={jsCode}>
                    <TableCell><code>0x{keyCode?.hidCode?.toString(16)?.padStart(2, "0") ?? "N/A"}</code></TableCell>
                    <TableCell>{keyCode?.hidDescription ?? "N/A"}<br /><em>{keyCode?.note}</em></TableCell>
                    <TableCell><code>{keyCode?.jsCode ?? jsCode}</code></TableCell>
                    <TableCell style={{ whiteSpace: "nowrap" }}
                               align={"right"}>{keyCode?.category ?? "Other"}</TableCell>
                  </TableRow>)
              })}
            </TableBody>
          </Table>
          <div style={{ flex: 1 }} />
        </Stack>
      </CardContent>
    </>

  )
}

const KeyboardLogCardContent = (props: {
  state: KeyboardState
}) => {
  const { state } = props
  const logItems = useMemo(() => state.eventLog.toReversed(), [state.eventLog])
  return (
    <>
      <CardHeader title={"Event Log"} subheader={`The last ${maxEventLogItems} events are shown. The most recent events are at the top.`} />
      <CardContent style={{ paddingLeft: 0, paddingRight: 0 }}>
        <Stack spacing={2} style={{ height: 400, overflow: "auto" }}>
          <Table size={"small"}>
            <TableHead>
              <TableRow>
                <TableCell style={{ width: 100, whiteSpace: "nowrap" }}>Time</TableCell>
                <TableCell style={{ width: 0, whiteSpace: "nowrap" }}>Event</TableCell>
                <TableCell style={{ width: 0, whiteSpace: "nowrap" }}>HID Code</TableCell>
                <TableCell style={{ whiteSpace: "nowrap" }}>HID Description</TableCell>
                <TableCell style={{ width: 150, whiteSpace: "nowrap" }}>JS Code</TableCell>
                <TableCell style={{ width: 200, whiteSpace: "nowrap" }} align={"right"}>Category</TableCell>
              </TableRow>
            </TableHead>
            <TableBody>
              {logItems.length === 0 && (
                <TableRow>
                  <TableCell colSpan={6}>
                    <center>
                      <em>No events recorded yet.</em>
                    </center>
                  </TableCell>
                </TableRow>
              )}
              {logItems.map(logItem => {
                const jsCode = logItem.jsCode
                const keyCode = keyboadKeyCodes.byJsCode[jsCode] ?? null
                return (
                  <TableRow key={logItem.timestamp.toMillis()}>
                    <TableCell>
                      {logItem.timestamp.toFormat("HH:mm:ss.SSS")}
                    </TableCell>
                    <TableCell style={{ whiteSpace: "nowrap" }}>
                      <center>
                        <Tooltip title={logItem.type}>
                          <Avatar
                            sx={{
                              width: 24,
                              height: 24,
                              bgcolor: logItem.type === "Pressed" ? blue["A400"] : deepPurple["A400"]
                            }}
                            variant={"circular"}
                          >
                            {logItem.type === "Pressed" ? <FileDownloadIcon fontSize={"small"} /> :
                              <FileUploadIcon fontSize={"small"} />}
                          </Avatar>
                        </Tooltip>
                      </center>
                    </TableCell>
                    <TableCell><code>0x{keyCode?.hidCode?.toString(16)?.padStart(2, "0") ?? "N/A"}</code></TableCell>
                    <TableCell>{keyCode?.hidDescription ?? "N/A"}<br /><em>{keyCode?.note}</em></TableCell>
                    <TableCell><code>{keyCode?.jsCode ?? jsCode}</code></TableCell>
                    <TableCell style={{ whiteSpace: "nowrap" }}
                               align={"right"}>{keyCode?.category ?? "Other"}</TableCell>
                  </TableRow>)
              })}
            </TableBody>
          </Table>
          <div style={{ flex: 1 }} />
        </Stack>
      </CardContent>
    </>

  )
}

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

function useKeyboardState(keyCaptureEnabled: boolean): KeyboardState {
  const [pressedKeyCodes, setPressedKeyCodes] = useState<string[]>([])
  const [eventLog, setEventLog] = useState<KeyboardEventLogItem[]>([])

  const handleReset = useCallback(() => {
    setPressedKeyCodes([])
    setEventLog([])
  }, [setPressedKeyCodes, setEventLog])

  useEffect(() => {
    const keyDownHandler = (e: KeyboardEvent) => {
      if (!e.repeat) {
        setPressedKeyCodes(s => [...s, e.code])

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
      setPressedKeyCodes(s => s.filter(it => it !== e.code))
      setEventLog(s => {
        const logItem = { timestamp: DateTime.now(), type: "Released", jsCode: e.code } satisfies KeyboardEventLogItem
        const next = [...s, logItem]
        return takeLast(next, maxEventLogItems)
      })
    }

    document.addEventListener("keydown", keyDownHandler)
    document.addEventListener("keyup", keyUpHandler)
    return () => {
      document.removeEventListener("keydown", keyDownHandler)
      document.removeEventListener("keyup", keyUpHandler)
    }
  }, [setPressedKeyCodes, setEventLog, keyCaptureEnabled])

  return {
    pressedKeyCodes,
    eventLog,
    reset: handleReset
  }
}

function takeLast<T>(arr: T[], count: number): T[] {
  return arr.length > count ? arr.slice(arr.length - count) : arr
}