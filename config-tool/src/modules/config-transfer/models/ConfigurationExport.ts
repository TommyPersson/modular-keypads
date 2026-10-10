import { type KeyBinding, KeyBindingTriggerType } from "@src/modules/key-bindings/models"
import { type MacroDefinition, MacroDefinitionType } from "@src/modules/macros/models"
import type { SettingChange } from "@src/modules/settings/commands"
import type { Setting } from "@src/modules/settings/models"

export const ConfigurationExportVersion = 1

export type ConfigurationExport = {
  version: number
  exportedAt: string
  macros: MacroDefinition[]
  keyBindings: KeyBinding[]
  // Stored as the same key/value strings that are sent to the device, so that new setting types need no format change
  settings: SettingChange[]
}

export function createConfigurationExport(
  macros: MacroDefinition[],
  keyBindings: KeyBinding[],
  settings: Setting[],
): ConfigurationExport {
  return {
    version: ConfigurationExportVersion,
    exportedAt: new Date().toISOString(),
    macros,
    keyBindings,
    settings: settings.map(({ key, value }) => ({ key, value: value.toString() })),
  }
}

// Only checks the structure needed to send the contents to the device, the device validates the rest
export function parseConfigurationExport(text: string): ConfigurationExport {
  let data: any
  try {
    data = JSON.parse(text)
  } catch {
    throw new Error("The file is not valid JSON")
  }

  if (typeof data !== "object" || data === null) {
    throw new Error("The file does not contain a configuration export")
  }

  if (data.version !== ConfigurationExportVersion) {
    throw new Error(`Unsupported configuration export version: ${data.version}`)
  }

  if (!Array.isArray(data.macros) || !Array.isArray(data.keyBindings)) {
    throw new Error("The file is missing macros or key bindings")
  }

  const macroTypes = Object.values(MacroDefinitionType) as string[]
  data.macros.forEach((macro: any, index: number) => {
    if (typeof macro?.id !== "number" || typeof macro?.name !== "string" || !macroTypes.includes(macro?.type)) {
      throw new Error(`Macro #${index + 1} is invalid`)
    }
  })

  const triggerTypes = Object.values(KeyBindingTriggerType) as string[]
  data.keyBindings.forEach((keyBinding: any, index: number) => {
    const trigger = keyBinding?.trigger
    if (
      typeof keyBinding?.macroId !== "number" ||
      typeof trigger?.deviceId !== "string" ||
      typeof trigger?.number !== "number" ||
      !triggerTypes.includes(trigger?.type)
    ) {
      throw new Error(`Key binding #${index + 1} is invalid`)
    }
  })

  // Exports made before settings were included have no settings
  data.settings ??= []
  if (!Array.isArray(data.settings)) {
    throw new Error("The file has invalid settings")
  }

  data.settings.forEach((setting: any, index: number) => {
    if (typeof setting?.key !== "string" || typeof setting?.value !== "string") {
      throw new Error(`Setting #${index + 1} is invalid`)
    }
  })

  return data as ConfigurationExport
}
