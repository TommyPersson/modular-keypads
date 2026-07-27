import HomeOutlinedIcon from "@mui/icons-material/HomeOutlined"
import type { AppModule } from "@src/modules/common/AppModule"
import { OverviewPage } from "@src/modules/overview/ui"

export const OverviewAppModule: AppModule = {
  id: "overview",
  order: 1,
  navItems: [
    {
      title: "Overview",
      link: "/overview",
      icon: <HomeOutlinedIcon />
    }
  ],
  routes: [
    {
      path: "overview",
      Component: OverviewPage,
    },
  ],
  toolPanes: [],
}