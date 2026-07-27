import KeyboardAltOutlinedIcon from "@mui/icons-material/KeyboardAltOutlined"
import KeyboardOutlinedIcon from "@mui/icons-material/KeyboardOutlined"
import type { AppModule } from "@src/modules/common/AppModule"
import { KeyBindingsPage } from "@src/modules/key-bindings/ui"
import { KeyboardEventsToolPaneContent } from "@src/modules/key-bindings/ui/components"

export const KeyBindingsAppModule: AppModule = {
  id: "key-bindings",
  order: 1,
  navItems: [
    {
      title: "Key Bindings",
      link: "/key-bindings",
      icon: <KeyboardOutlinedIcon />
    }
  ],
  routes: [
    {
      path: "key-bindings",
      Component: KeyBindingsPage,
    }
  ],
  toolPanes: [
    {
      id: "keyboard-events",
      title: "Keyboard Events",
      component: <KeyboardEventsToolPaneContent />,
      icon: <KeyboardAltOutlinedIcon />
    },
  ]
}