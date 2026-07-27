import type { AppModule } from "@src/modules/common/AppModule"
import { DeviceDebuggerAppModule } from "@src/modules/device-debugger/DeviceDebuggerAppModule"
import { DeviceAppModule } from "@src/modules/device/DeviceAppModule"
import { KeyBindingsAppModule } from "@src/modules/key-bindings/KeyBindingsAppModule"
import { MacrosAppModule } from "@src/modules/macros/MacrosAppModule"
import { OverviewAppModule } from "@src/modules/overview/OverviewAppModule"

export const AllAppModules: AppModule[] = [
  OverviewAppModule,
  KeyBindingsAppModule,
  MacrosAppModule,
  DeviceAppModule,
  DeviceDebuggerAppModule,
].toSorted((a, b) => a.order - b.order)