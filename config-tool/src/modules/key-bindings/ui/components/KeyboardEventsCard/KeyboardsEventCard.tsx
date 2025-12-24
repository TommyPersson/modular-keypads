import {
  Alert,
  Button,
  Card,
  CardContent,
  CardHeader,
  Stack,
  Table, TableBody,
  TableCell,
  TableHead, TableRow,
  ToggleButton,
  Typography
} from "@mui/material"
import { useDeviceFacade } from "@src/modules/device/context"
import { keyboadKeyCodes } from "@src/modules/key-bindings/data"
import { useCallback, useEffect, useState } from "react"

export const KeyboardEventsCard = () => {
  const deviceFacade = useDeviceFacade()

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
            <ToggleButton onClick={handleKeyCaptureToggled} selected={keyCaptureEnabled}>Key Capture</ToggleButton>
            <Button onClick={keyboardState.reset}>Reset</Button>
          </>
        }
      />
      <CardContent>
        <Stack spacing={2} style={{ minHeight: 400 }}>
          <Typography variant={"subtitle2"}>
            Currently Pressed Keys
          </Typography>
          <Table>
            <TableHead>
              <TableRow>
                <TableCell style={{ width: 0, whiteSpace: "nowrap" }}>HID Code</TableCell>
                <TableCell>HID Description</TableCell>
                <TableCell style={{ width: 150, whiteSpace: "nowrap" }}>JS Code</TableCell>
                <TableCell style={{ width: 0, whiteSpace: "nowrap" }} align={"right"}>Category</TableCell>
              </TableRow>
            </TableHead>
            <TableBody>
              {keyboardState.pressedKeyCodes.map(jsCode => {
                const keyCode = keyboadKeyCodes.byJsCode[jsCode] ?? null
                return (
                  <TableRow key={jsCode}>
                    <TableCell><code>0x{keyCode?.hidCode?.toString(16)?.padStart(2, "0") ?? "N/A"}</code></TableCell>
                    <TableCell>{keyCode?.hidDescription ?? "N/A"}<br/><em>{keyCode?.note}</em></TableCell>
                    <TableCell><code>{keyCode?.jsCode ?? jsCode}</code></TableCell>
                    <TableCell style={{ whiteSpace: "nowrap" }} align={"right"}>{keyCode?.category ?? "Other"}</TableCell>
                  </TableRow>)
              })}
            </TableBody>
          </Table>
          <div style={{ flex: 1 }} />
          <Alert color={"info"}>
            It is possible for this view to get out of sync with the keyboard if the pressed combination shifts focus
            away from the window. In that case you may reset the state of the preview.
            <p />
            You may also use the "Key Capture" button to prevent the browser from handling the pressed keys.
          </Alert>
        </Stack>
      </CardContent>
    </Card>
  )
}

type KeyboardState = {
  pressedKeyCodes: string[]
  reset(): void
}

function useKeyboardState(keyCaptureEnabled: boolean): KeyboardState {
  const [pressedKeyCodes, setPressedKeyCodes] = useState<string[]>([])

  const handleReset = useCallback(() => {
    setPressedKeyCodes([])
  }, [setPressedKeyCodes])

  useEffect(() => {
    const keyDownHandler = (e: KeyboardEvent) => {
      setPressedKeyCodes(s => {
        if (s.includes(e.code)) {
          return s // Prevent key repeats when keys are held
        } else {
          return [...s, e.code]
        }
      })

      if (keyCaptureEnabled) {
        e.preventDefault()
        e.stopPropagation()
        e.stopImmediatePropagation()
      }
    }

    const keyUpHandler = (e: KeyboardEvent) => {
      setPressedKeyCodes(s => s.filter(it => it !== e.code))
    }

    document.addEventListener("keydown", keyDownHandler)
    document.addEventListener("keyup", keyUpHandler)
    return () => {
      document.removeEventListener("keydown", keyDownHandler)
      document.removeEventListener("keyup", keyUpHandler)
    }
  }, [setPressedKeyCodes, keyCaptureEnabled])

  return {
    pressedKeyCodes,
    reset: handleReset
  }
}
