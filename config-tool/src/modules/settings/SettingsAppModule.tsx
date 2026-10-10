import SettingsOutlinedIcon from "@mui/icons-material/SettingsOutlined"
import type { AppModule } from "@src/modules/common/AppModule"
import { SettingsPage } from "@src/modules/settings/ui"

export const SettingsAppModule: AppModule = {
  id: "settings",
  order: 900,
  navItems: [
    {
      title: "Settings",
      link: "/settings",
      icon: <SettingsOutlinedIcon />
    }
  ],
  routes: [
    {
      path: "settings",
      Component: SettingsPage,
    }
  ],
  toolPanes: [],
}
