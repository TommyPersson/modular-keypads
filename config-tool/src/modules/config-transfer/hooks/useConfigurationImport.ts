import type { ConfigurationExport } from "@src/modules/config-transfer/models"
import { SetKeyBindingCommand } from "@src/modules/key-bindings/commands"
import { ListKeyBindingsQueryKey } from "@src/modules/key-bindings/queries"
import { SaveMacroCommand } from "@src/modules/macros/commands"
import { ListStoredMacrosQueryKey } from "@src/modules/macros/queries"
import { SaveSettingsCommand } from "@src/modules/settings/commands"
import { ListSettingsQueryKey } from "@src/modules/settings/queries"
import { queryClient } from "@src/utils/queryClient"
import { useMutation } from "@tanstack/react-query"
import { useCallback, useRef, useState } from "react"

export enum ConfigurationImportStatus {
  Idle = "Idle",
  Running = "Running",
  Cancelling = "Cancelling",
  Completed = "Completed",
  Cancelled = "Cancelled",
}

export type ConfigurationImportFailure = {
  step: string
  error: Error
}

export type ConfigurationImportProgress = {
  status: ConfigurationImportStatus
  // Number of processed items, including failed ones
  completed: number
  total: number
  currentStep: string
  failures: ConfigurationImportFailure[]
}

const InitialProgress: ConfigurationImportProgress = {
  status: ConfigurationImportStatus.Idle,
  completed: 0,
  total: 0,
  currentStep: "",
  failures: [],
}

export type UseConfigurationImportResult = {
  progress: ConfigurationImportProgress
  start: (configuration: ConfigurationExport) => Promise<void>
  cancel: () => void
  reset: () => void
}

// Writes macros, key bindings and then settings, one device command at a time, so that progress can be reported.
// A failing item is recorded and skipped, so that the rest of the configuration is still imported
export function useConfigurationImport(): UseConfigurationImportResult {
  // The per-item invalidations are skipped, the lists are refreshed once when the import is done
  const { mutateAsync: saveMacro } = useMutation({ ...SaveMacroCommand.mutationOptions, onSuccess: undefined })
  const { mutateAsync: setKeyBinding } = useMutation({ ...SetKeyBindingCommand.mutationOptions, onSuccess: undefined })
  const { mutateAsync: saveSettings } = useMutation({ ...SaveSettingsCommand.mutationOptions, onSettled: undefined })

  const [progress, setProgress] = useState<ConfigurationImportProgress>(InitialProgress)
  const cancelRequested = useRef(false)

  const start = useCallback(async (configuration: ConfigurationExport) => {
    const { macros, keyBindings, settings } = configuration

    cancelRequested.current = false
    setProgress({
      ...InitialProgress,
      status: ConfigurationImportStatus.Running,
      total: macros.length + keyBindings.length + settings.length,
    })

    const steps: { description: string, execute: () => Promise<unknown> }[] = [
      ...macros.map(macro => ({
        description: `Saving macro '${macro.name}'`,
        execute: async () => {
          console.log("macro", macro)
          if (SaveMacroCommand.canExecute?.({ macro }) === false) {
            throw new Error(`The macro '${macro.name}' is incomplete`)
          }
          await saveMacro({ macro })
        },
      })),
      ...keyBindings.map(({ trigger, macroId }, index) => ({
        description: `Saving key binding ${index + 1} of ${keyBindings.length}`,
        execute: () => setKeyBinding({ trigger, macroId }),
      })),
      ...settings.map(setting => ({
        description: `Saving setting '${setting.key}'`,
        execute: () => saveSettings({ changes: [setting] }),
      })),
    ]

    let completed = 0
    try {
      for (const step of steps) {
        if (cancelRequested.current) {
          break
        }
        setProgress(previous => ({ ...previous, completed, currentStep: step.description }))
        try {
          await step.execute()
        } catch (error) {
          const failure = { step: step.description, error: error as Error }
          setProgress(previous => ({ ...previous, failures: [...previous.failures, failure] }))
        }
        completed++
      }

      setProgress(previous => ({
        ...previous,
        completed,
        currentStep: "",
        status: completed < steps.length ? ConfigurationImportStatus.Cancelled : ConfigurationImportStatus.Completed,
      }))
    } finally {
      await queryClient.invalidateQueries({ queryKey: ListStoredMacrosQueryKey })
      await queryClient.invalidateQueries({ queryKey: ListKeyBindingsQueryKey })
      await queryClient.invalidateQueries({ queryKey: ListSettingsQueryKey })
    }
  }, [saveMacro, setKeyBinding, saveSettings])

  const cancel = useCallback(() => {
    cancelRequested.current = true
    setProgress(previous => ({ ...previous, status: ConfigurationImportStatus.Cancelling }))
  }, [])

  const reset = useCallback(() => {
    setProgress(InitialProgress)
  }, [])

  return { progress, start, cancel, reset }
}
