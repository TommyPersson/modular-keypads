import type { AppModule } from "@src/modules/common/AppModule"
import { OverviewPage } from "@src/modules/overview/ui"

export const OverviewAppModule: AppModule = {
  id: "overview",
  order: 1,
  routes: [
    {
      path: "overview",
      Component: OverviewPage,
    },
  ],
  toolPanes: [],
}