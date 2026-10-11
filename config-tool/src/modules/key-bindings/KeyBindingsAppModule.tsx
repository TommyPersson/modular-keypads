import KeyboardAltOutlinedIcon from "@mui/icons-material/KeyboardAltOutlined"
import type { AppModule } from "@src/modules/common/AppModule"
import { KeyboardEventsToolPaneContent } from "@src/modules/key-bindings/ui/components"

export const KeyBindingsAppModule: AppModule = {
  id: "key-bindings",
  order: 1,
  routes: [],
  toolPanes: [
    {
      id: "keyboard-events",
      title: "Keyboard Events",
      component: <KeyboardEventsToolPaneContent />,
      icon: <KeyboardAltOutlinedIcon />
    },
  ]
}
