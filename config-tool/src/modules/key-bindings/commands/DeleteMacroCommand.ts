import { globalDeviceFacade } from "@src/modules/device/context"
import { ListStoredMacrosQueryKey } from "@src/modules/key-bindings/queries"
import { queryClient } from "@src/utils/queryClient"
import type { UseMutationOptions } from "@tanstack/react-query"

export const DeleteMacroMutation: UseMutationOptions<void, Error, { id: number }> = {
  mutationFn: async ({ id }) => {
    await globalDeviceFacade.value.deleteMacro(id)
  },
  onSuccess: async () => {
    await queryClient.invalidateQueries({ queryKey: ListStoredMacrosQueryKey })
  }
}