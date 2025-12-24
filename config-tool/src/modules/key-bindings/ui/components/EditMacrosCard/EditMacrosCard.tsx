import { Card, CardContent, CardHeader } from "@mui/material"
import { useDeviceFacade } from "@src/modules/device/context"

export const EditMacrosCard = () => {
  const deviceFacade = useDeviceFacade()

  return (
    <Card>
      <CardHeader title={"Macros"} />
      <CardContent>
      </CardContent>
    </Card>
  )
}

type KeyActionType = "press" | "release"
