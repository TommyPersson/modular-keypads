import KeyboardCommandKeyOutlinedIcon from "@mui/icons-material/KeyboardCommandKeyOutlined"
import type { AppModule } from "@src/modules/common/AppModule"
import { MacrosPage } from "@src/modules/macros/ui"

export const MacrosAppModule: AppModule = {
  id: "macros",
  order: 3,
  navItems: [
    {
      title: "Macros",
      link: "/macros",
      icon: <KeyboardCommandKeyOutlinedIcon />
    }
  ],
  routes: [
    {
      path: "macros",
      Component: MacrosPage,
    }
  ],
  toolPanes: [],
}
