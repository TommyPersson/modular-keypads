import UploadFileOutlinedIcon from "@mui/icons-material/UploadFileOutlined"
import {
  Alert,
  Button,
  Dialog,
  DialogActions,
  DialogContent,
  DialogContentText,
  DialogTitle,
  LinearProgress,
  Stack,
  Typography
} from "@mui/material"
import { useDeviceFacade } from "@src/modules/device/context"
import {
  type FirmwareUpdateProgress,
  globalFirmwareUpdateController as controller,
  isFirmwareUpdateRunning,
  useFirmwareUpdateState
} from "@src/modules/device/firmware"
import { type ChangeEvent, type DragEvent, useCallback, useState } from "react"

const phaseLabels: Record<FirmwareUpdateProgress["phase"], string> = {
  uploading: "Uploading",
  verifying: "Verifying",
  restarting: "Restarting",
  done: "Updated",
}

/**
 * Updates the connected device with a firmware image picked by the user. Rendered once, outside of the device
 * previews, as they're removed while the device restarts. It can't be closed while the update is running.
 */
export const UpdateFirmwareDialog = () => {
  const facade = useDeviceFacade()
  const state = useFirmwareUpdateState()
  const { isOpen, deviceName, file, fileError, progress, error } = state

  const isRunning = isFirmwareUpdateRunning(state)
  const isDone = progress?.phase === "done"

  const handleFileChange = useCallback((event: ChangeEvent<HTMLInputElement>) => {
    controller.selectFile(event.target.files?.[0] ?? null)
    // Cleared, so that picking the same file again, e.g. after rebuilding it, is noticed
    event.target.value = ""
  }, [])

  const [isDragOver, setIsDragOver] = useState(false)

  // Files can't be swapped while an update is running, so they're not accepted then
  const handleDragOver = useCallback((event: DragEvent) => {
    event.preventDefault()
    event.dataTransfer.dropEffect = isRunning ? "none" : "copy"
    setIsDragOver(!isRunning)
  }, [isRunning])

  // Moving onto the button or the file name also leaves the drop zone itself, which isn't leaving it
  const handleDragLeave = useCallback((event: DragEvent) => {
    if (!event.currentTarget.contains(event.relatedTarget as Node | null)) {
      setIsDragOver(false)
    }
  }, [])

  const handleDrop = useCallback((event: DragEvent) => {
    event.preventDefault()
    setIsDragOver(false)

    const droppedFile = event.dataTransfer.files[0]
    if (droppedFile) {
      controller.selectFile(droppedFile)
    }
  }, [])

  const handleUpdate = useCallback(() => {
    controller.start(facade).then()
  }, [facade])

  const handleCancel = useCallback(() => controller.cancel(), [])
  const handleClose = useCallback(() => controller.close(), [])

  const percent = progress ? Math.round(100 * progress.writtenBytes / progress.totalBytes) : 0

  return (
    <Dialog open={isOpen} onClose={handleClose} fullWidth maxWidth={"sm"}>
      <DialogTitle>Update Firmware</DialogTitle>
      <DialogContent>
        <Stack spacing={2}>
          <DialogContentText>
            Updates <strong>{deviceName}</strong> with a firmware image, the <code>firmware.bin</code> built by
            PlatformIO. The device restarts into the new firmware once it has been verified, and is reconnected
            automatically.
          </DialogContentText>

          {/* The file can also be dropped here, e.g. straight from the build folder */}
          <Stack
            direction={"row"}
            spacing={2}
            alignItems={"center"}
            onDragOver={handleDragOver}
            onDragLeave={handleDragLeave}
            onDrop={handleDrop}
            sx={{
              p: 2,
              border: 2,
              borderStyle: "dashed",
              borderRadius: 1,
              borderColor: isDragOver ? "primary.main" : "divider",
              bgcolor: isDragOver ? "action.hover" : undefined,
              transition: "border-color 150ms, background-color 150ms",
            }}
          >
            <Button
              component={"label"}
              variant={"outlined"}
              startIcon={<UploadFileOutlinedIcon />}
              disabled={isRunning}
              sx={{ flexShrink: 0 }}
            >
              Select file
              <input type={"file"} accept={".bin"} hidden onChange={handleFileChange} />
            </Button>
            <Typography variant={"body2"} color={"text.secondary"} noWrap>
              {file ? `${file.name} (${Math.round(file.size / 1024)} kB)` : "or drop it here"}
            </Typography>
          </Stack>

          {progress && (
            <Stack spacing={0.5}>
              <LinearProgress
                variant={progress.phase === "uploading" || isDone || error ? "determinate" : "indeterminate"}
                value={isDone ? 100 : percent}
                color={error ? "error" : isDone ? "success" : "primary"}
              />
              <Typography variant={"caption"} color={"text.secondary"}>
                {phaseLabels[progress.phase]}
                {progress.phase === "uploading" && ` ${percent}%`}
              </Typography>
            </Stack>
          )}

          {fileError && <Alert severity={"warning"}>{fileError}</Alert>}
          {error && <Alert severity={"error"}>{error}</Alert>}
          {isDone && <Alert severity={"success"}>The firmware was updated</Alert>}
        </Stack>
      </DialogContent>
      <DialogActions>
        {isRunning
          ? <Button onClick={handleCancel} disabled={progress?.phase !== "uploading"}>Cancel</Button>
          : <Button onClick={handleClose}>Close</Button>}
        <Button variant={"contained"} onClick={handleUpdate} disabled={!file || fileError !== null || isRunning || isDone}>
          Update
        </Button>
      </DialogActions>
    </Dialog>
  )
}
