import { Button, Card, CardContent, Typography } from "@mui/material"


export const PingCommandCard = () => {
  return (
    <Card>
      <CardContent>
        <Typography variant="h5" component="div">Ping Test</Typography>
        <Button>Ping!</Button>
      </CardContent>
    </Card>
  )
}