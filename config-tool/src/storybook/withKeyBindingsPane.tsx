import { Stack } from "@mui/material"
import {
  KeyBindingSelectionProvider,
  KeyBindingsPane,
  type KeyBindingTarget
} from "@src/modules/key-bindings/ui/components/KeyBindingsPane"
import { KEY_BINDINGS_PANE_HEIGHT } from "@src/modules/root/ui/overlayLayout"
import type { Decorator } from "@storybook/react-vite"

/**
 * Shows the key bindings pane below the story, as on the overview page, so that the inputs in the story can be
 * selected to edit their bindings. The input that is selected at first is given by the keyBindingsPane parameter.
 * Needs the mock device, so it's placed before withMockDevice in the decorators.
 */
export const withKeyBindingsPane: Decorator = (Story, context) => {
  const initialSelectedTarget: KeyBindingTarget | null = context.parameters.keyBindingsPane?.selectedTarget ?? null

  // Keyed by story, so that switching stories selects the story's input
  return (
    <KeyBindingSelectionProvider key={context.id} initialSelectedTarget={initialSelectedTarget}>
      <Stack spacing={2} alignItems={"center"}>
        <Story />
        <div style={{ width: 800, height: KEY_BINDINGS_PANE_HEIGHT }}>
          <KeyBindingsPane />
        </div>
      </Stack>
    </KeyBindingSelectionProvider>
  )
}
