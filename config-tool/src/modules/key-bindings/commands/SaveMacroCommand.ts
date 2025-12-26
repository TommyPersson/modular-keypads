import { globalDeviceFacade } from "@src/modules/device/context"
import type { MacroDefinition } from "@src/modules/key-bindings/models"
import { ListStoredMacrosQueryKey } from "@src/modules/key-bindings/queries"
import { queryClient } from "@src/utils/queryClient"
import type { UseMutationOptions } from "@tanstack/react-query"

export const SaveMacroMutation: UseMutationOptions<void, Error, { macro: MacroDefinition }> = {
  mutationFn: async ({ macro }) => {
    await globalDeviceFacade.value.saveMacro(macro)
  },
  onSuccess: async () => {
    await queryClient.invalidateQueries({ queryKey: ListStoredMacrosQueryKey })
  }
}