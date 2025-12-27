import { Button, type ButtonProps, IconButton, Popover, Tooltip } from "@mui/material"
import { type Command, executeCommand } from "@src/utils/commands"
import { useConfirm } from "@src/utils/hooks/useConfirm"
import { useMutation } from "@tanstack/react-query"
import { useRef } from "react"
import { ErrorAlert } from "../ErrorAlert"


export type CommandButtonProps<TArgs> = {
  command: Command<TArgs>
  args: TArgs | null
  onSuccess?: () => void
  iconOnly?: boolean
} & ButtonProps

export const CommandButton = <TArgs, >(props: CommandButtonProps<TArgs>) => {
  const { command, args, onSuccess, ...buttonProps } = props

  const confirm = useConfirm()

  const mutation = useMutation(command.mutationOptions)

  const buttonRef = useRef<HTMLButtonElement | null>(null)

  const handleClick = async () => {
    await executeCommand({ command, mutation, args, confirm, onSuccess })
  }

  const disabled = props.disabled || args === null || command.canExecute?.(args) === false

  const button = props.iconOnly ? (
    <Tooltip title={command.label}>
      <IconButton
        ref={buttonRef}
        {...buttonProps}
        color={command.color}
        onClick={handleClick}
        loading={mutation.isPending}
        disabled={disabled}
      >
        {command.icon}
      </IconButton>
    </Tooltip>
  ) : (
    <Button
      ref={buttonRef}
      {...buttonProps}
      color={command.color}
      children={command.label}
      startIcon={command.icon}
      onClick={handleClick}
      loading={mutation.isPending}
      disabled={disabled}
    />
  )

  return (
    <>
      {button}
      <Popover
        open={!!mutation.error}
        anchorEl={buttonRef.current}
        anchorOrigin={{ vertical: "bottom", horizontal: "left" }}
        onClose={mutation.reset}
      >
        <ErrorAlert title={command.errorTitle} error={mutation.error} onClose={mutation.reset} />
      </Popover>
    </>
  )
}
