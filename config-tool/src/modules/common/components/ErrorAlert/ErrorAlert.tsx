import { Alert, AlertTitle } from "@mui/material"

export type ErrorAlertProps = {
  readonly error: Error | null
  readonly title?: string | null
  readonly onClose?: () => void
}

export const ErrorAlert = (props: ErrorAlertProps) => {
  const { error, onClose } = props

  if (!error) {
    return null
  }

  const title = props.title || "An error occurred"

  return (
    <Alert severity={"error"} onClose={onClose}>
      <AlertTitle>{title}</AlertTitle>
      {error.message}
    </Alert>
  )
}