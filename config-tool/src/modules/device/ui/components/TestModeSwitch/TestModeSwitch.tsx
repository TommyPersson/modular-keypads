import { FormControlLabel, Switch, Tooltip } from "@mui/material"
import { useDeviceFacade } from "@src/modules/device/context"
import { SetTestModeDeviceCommand } from "@src/modules/device/facade/device-commands/SetTestModeDeviceCommand"
import { GetTestModeQuery } from "@src/modules/device/queries/GetTestModeQuery"
import { useMutation, useQuery, useQueryClient } from "@tanstack/react-query"
import { useCallback } from "react"

export const TestModeSwitch = () => {
  const deviceFacade = useDeviceFacade()
  const queryClient = useQueryClient()

  const testModeQuery = useQuery(GetTestModeQuery(deviceFacade))
  const testMode = testModeQuery.data ?? false

  const toggleTestMode = useMutation({
      mutationFn: async (args: { enabled: boolean }) => {
        await deviceFacade.executeCommand(new SetTestModeDeviceCommand(args.enabled))
      },
      onSuccess: async () => {
        await queryClient.invalidateQueries({ queryKey: ["test-mode-state"] })
      }
    }
  )

  const handleTestModeToggled = useCallback(async () => {
    await toggleTestMode.mutateAsync({ enabled: !testMode })
  }, [testMode, toggleTestMode])

  return (
    <Tooltip title={"No keybindings will be executed while test mode is active"}>
      <FormControlLabel
        label={"Test Mode"}
        sx={{ ml: 0.5, mr: 1 }}
        control={
          <Switch
            checked={testMode}
            color={"warning"}
            disabled={!testModeQuery.isSuccess || toggleTestMode.isPending}
            onChange={handleTestModeToggled}
          />
        }
      />
    </Tooltip>
  )
}
