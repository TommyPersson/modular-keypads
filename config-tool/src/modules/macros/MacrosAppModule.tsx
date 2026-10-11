import KeyboardCommandKeyOutlinedIcon from "@mui/icons-material/KeyboardCommandKeyOutlined"
import type { AppModule } from "@src/modules/common/AppModule"
import { MacrosToolPaneContent } from "@src/modules/macros/ui"

export const MacrosAppModule: AppModule = {
  id: "macros",
  order: 3,
  navItems: [],
  routes: [],
  toolPanes: [
    {
      id: "macros",
      title: "Macros",
      component: <MacrosToolPaneContent />,
      icon: <KeyboardCommandKeyOutlinedIcon />
    }
  ],
}
