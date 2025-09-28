import { Alert, Card, CardContent, Typography } from "@mui/material"
import { useEffect, useRef, useState } from "react"
import { useDeviceContext } from "../../context/DeviceContext"
import type { LogMessage } from "../../facade/DeviceFacade"

export const LogsCard = () => {

  const deviceContext = useDeviceContext()
  const { facade: deviceFacade } = deviceContext

  const scrollViewRef = useRef<HTMLPreElement | null>(null)
  const [output, setOutput] = useState<LogMessage[]>([])

  useEffect(() => {
    const subscription = deviceFacade.$logs.subscribe(next => {
      setOutput(s => [...s, next])
    })

    return () => {
      subscription.unsubscribe()
    }
  }, [deviceFacade.$logs, setOutput])

  useEffect(() => {
    setTimeout(() => {
      const scrollView = scrollViewRef.current
      if (scrollView) {
        scrollView.scrollTo({ top: scrollView.scrollHeight })
      }

    }, 0)


  }, [output])

  return (
    <Card>
      <CardContent>
        <Typography variant="h5" component="div">Message Logs</Typography>
        <pre style={{
          overflowY: "scroll",
          minHeight: 100,
          maxHeight: 400,
          background: 'lightgray',
          padding: 8,
        }} ref={scrollViewRef}>
            {output.map(it => `${formatLogLine(it)}`)}
        </pre>
      </CardContent>
    </Card>
  )
}

function formatLogLine(message: LogMessage) {
  if (message.direction === 'to-host') {
    return `< ${message.message}\n`
  } else {
    return `> ${message.message}\n`
  }
}