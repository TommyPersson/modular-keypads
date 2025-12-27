import { type ButtonProps,  type IconProps } from "@mui/material"
import type { UseConfirmResult } from "@src/utils/hooks/useConfirm"
import { type UseMutationOptions, type UseMutationResult } from "@tanstack/react-query"
import * as React from "react"

export type Command<TArgs> = {
  mutationOptions: UseMutationOptions<any, Error, TArgs>,
  label: any
  color?: ButtonProps["color"]
  icon?: React.ReactElement<IconProps>
  errorTitle?: string
  confirmText?: React.ReactElement | ((args: TArgs) => React.ReactElement)
  confirmColor?: ButtonProps["color"]
  canExecute?: (args: TArgs) => boolean
}

export async function executeCommand<TArgs>(options: {
  command: Command<TArgs>,
  mutation: UseMutationResult<any, Error, TArgs>,
  args: TArgs | null,
  confirm: UseConfirmResult,
  onSuccess?: () => void,
}) {
  const {
    command,
    mutation,
    args,
    confirm,
    onSuccess,
  } = options

  if (!args || command?.canExecute?.(args) === false) {
    return
  }

  if (command.confirmText) {
    let content = command.confirmText
    if (typeof command.confirmText === "function") {
      content = command.confirmText(args)
    }

    const { confirmed } = await confirm({
      content: content,
      color: command.confirmColor,
    })

    if (!confirmed) {
      return
    }
  }

  await mutation.mutateAsync(args)
  onSuccess?.()
}