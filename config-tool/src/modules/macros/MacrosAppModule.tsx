import KeyboardCommandKeyOutlinedIcon from "@mui/icons-material/KeyboardCommandKeyOutlined"
import type { AppModule } from "@src/modules/common/AppModule"
import { MacrosToolPaneContent } from "@src/modules/macros/ui"

export const MacrosAppModule: AppModule = {
  id: "macros",
  order: 3,
  routes: [],
  toolPanes: [
    {
      id: "macros",
      title: "Macros",
      shortTitle: "Macros",
      component: <MacrosToolPaneContent />,
      icon: <KeyboardCommandKeyOutlinedIcon />
    }
  ],
}
