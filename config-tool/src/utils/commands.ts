import { type ButtonProps, type IconProps } from "@mui/material"
import { useConfirm, type UseConfirmResult } from "@src/utils/hooks/useConfirm"
import { useMutation, type UseMutationOptions, type UseMutationResult } from "@tanstack/react-query"
import { useCallback } from "react"
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

export type UseCommandHook<TArgs> = {
  executeAsync: (args: TArgs) => Promise<void>
  isPending: boolean
  error: Error | null
}

export type UseCommandOptions = {
  onSuccess?: () => void
}

export function useCommand<TArgs>(command: Command<TArgs>, options: UseCommandOptions = {}): UseCommandHook<TArgs> {
  const { onSuccess } = options

  const mutation = useMutation(command.mutationOptions)

  const confirm = useConfirm()

  const executeAsync = useCallback(async (args: TArgs) => {
    await executeCommand({ command, mutation, args, confirm, onSuccess })
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [command, onSuccess])

  return {
    executeAsync,
    isPending: mutation.isPending,
    error: mutation.error,
  }
}