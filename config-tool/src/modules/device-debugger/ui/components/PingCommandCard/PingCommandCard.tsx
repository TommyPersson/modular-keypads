import { Button, Card, CardContent, Typography } from "@mui/material"
import { useDeviceFacade } from "@src/modules/device/context"
import { useCallback, useState } from "react"


export const PingCommandCard = () => {
  const deviceFacade = useDeviceFacade()

  const [response, setResponse] = useState<string | null>(null)

  const handleClick = useCallback(async () => {
    setResponse(null)
    const response = await deviceFacade.performPing()
    setResponse(response)
  }, [deviceFacade, setResponse])

  return (
    <Card>
      <CardContent>
        <Typography variant="h5" component="div">Ping Test</Typography>
        <Button onClick={handleClick}>Ping!</Button>
        {response && <Typography>Response: <strong>{response}</strong></Typography>}
      </CardContent>
    </Card>
  )
}