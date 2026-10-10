import {
  Alert,
  AlertTitle,
  Button,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  LinearProgress,
  List,
  ListItem,
  ListItemText,
  Stack,
  Typography
} from "@mui/material"
import {
  type ConfigurationImportProgress,
  ConfigurationImportStatus
} from "@src/modules/config-transfer/hooks"
import { useCallback } from "react"

export const ImportConfigurationDialog = (props: {
  progress: ConfigurationImportProgress
  onCancel: () => void
  onClose: () => void
}) => {
  const { progress, onCancel, onClose } = props
  const { status, completed, total, currentStep, failures } = progress

  const isActive = status === ConfigurationImportStatus.Running || status === ConfigurationImportStatus.Cancelling
  const saved = completed - failures.length

  const handleClose = useCallback(() => {
    if (!isActive) {
      onClose()
    }
  }, [isActive, onClose])

  const percent = total > 0 ? (completed / total) * 100 : 0

  const progressColor = (() => {
    if (status === ConfigurationImportStatus.Cancelled) {
      return "warning"
    }
    if (failures.length > 0) {
      return "error"
    }
    return status === ConfigurationImportStatus.Completed ? "success" : "primary"
  })()

  return (
    <Dialog open={status !== ConfigurationImportStatus.Idle} fullWidth onClose={handleClose}>
      <DialogTitle>Import Configuration</DialogTitle>
      <DialogContent>
        <Stack spacing={2}>
          <LinearProgress variant={"determinate"} value={percent} color={progressColor} />
          <Typography variant={"body2"} color={"textSecondary"}>
            {saved} of {total} items saved
            {failures.length > 0 && <>, {failures.length} failed</>}
            {isActive && currentStep && <> &mdash; {currentStep}...</>}
          </Typography>
          {status === ConfigurationImportStatus.Completed && failures.length === 0 && (
            <Alert severity={"success"}>The configuration was imported successfully.</Alert>
          )}
          {status === ConfigurationImportStatus.Completed && failures.length > 0 && (
            <Alert severity={"warning"}>The configuration was imported, but some items could not be saved.</Alert>
          )}
          {status === ConfigurationImportStatus.Cancelled && (
            <Alert severity={"warning"}>The import was cancelled, the device has only been partially updated.</Alert>
          )}
          {failures.length > 0 && (
            <Alert severity={"error"} sx={{ "& .MuiAlert-message": { flexGrow: 1 } }}>
              <AlertTitle>Failed items</AlertTitle>
              <List dense disablePadding style={{ maxHeight: 240, overflow: "auto" }}>
                {failures.map((failure, index) => (
                  <ListItem key={index} disableGutters>
                    <ListItemText primary={failure.step} secondary={failure.error.message} />
                  </ListItem>
                ))}
              </List>
            </Alert>
          )}
        </Stack>
      </DialogContent>
      <DialogActions>
        {isActive ? (
          <Button onClick={onCancel} loading={status === ConfigurationImportStatus.Cancelling}>Cancel</Button>
        ) : (
          <Button variant={"contained"} onClick={handleClose}>Close</Button>
        )}
      </DialogActions>
    </Dialog>
  )
}
