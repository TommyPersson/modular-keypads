import SettingsOutlinedIcon from "@mui/icons-material/SettingsOutlined"
import type { AppModule } from "@src/modules/common/AppModule"
import { SettingsToolPaneContent } from "@src/modules/settings/ui"

export const SettingsAppModule: AppModule = {
  id: "settings",
  order: 900,
  navItems: [],
  routes: [],
  toolPanes: [
    {
      id: "settings",
      title: "Settings",
      component: <SettingsToolPaneContent />,
      icon: <SettingsOutlinedIcon />
    }
  ],
}
