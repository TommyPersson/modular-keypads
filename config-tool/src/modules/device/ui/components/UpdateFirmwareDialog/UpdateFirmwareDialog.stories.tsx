import { globalFirmwareUpdateController } from "@src/modules/device/firmware"
import { withMockDevice } from "@src/storybook"
import type { Meta, StoryObj } from "@storybook/react-vite"
import { useEffect } from "react"
import { UpdateFirmwareDialog } from "./UpdateFirmwareDialog"

// Opened when shown, as the dialog is otherwise opened from the connected device's preview card
const OpenUpdateFirmwareDialog = () => {
  useEffect(() => {
    globalFirmwareUpdateController.open("Master")
    return () => globalFirmwareUpdateController.close()
  }, [])

  return <UpdateFirmwareDialog />
}

const meta = {
  title: "Device/Update Firmware Dialog",
  component: OpenUpdateFirmwareDialog,
  decorators: [withMockDevice],
} satisfies Meta<typeof OpenUpdateFirmwareDialog>

export default meta

type Story = StoryObj<typeof meta>

// The mock device accepts any image, so the update completes as soon as it's uploaded
export const Default: Story = {}
