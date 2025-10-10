import { Card, CardContent, CardHeader } from "@mui/material"
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
      <CardHeader title={"Message Logs"} />
      <CardContent>
        <pre style={{
          overflowY: "scroll",
          minHeight: 100,
          maxHeight: 400,
          background: "lightgray",
          padding: 8,
        }} ref={scrollViewRef}>
            {output.map(it => `${formatLogLine(it)}`)}
        </pre>
      </CardContent>
    </Card>
  )
}

function formatLogLine(message: LogMessage) {
  if (message.direction === "to-host") {
    return `[${message.timestamp.toFormat("HH:mm:ss.SSS")}] < ${message.message}\n`
  } else {
    return `[${message.timestamp.toFormat("HH:mm:ss.SSS")}] > ${message.message}\n`
  }
}