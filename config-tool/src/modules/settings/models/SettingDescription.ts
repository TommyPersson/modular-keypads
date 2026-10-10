export type SettingDescription = {
  title: string
  description: string
  unit?: string
}

// Settings without a description are still listed, using their key as title
export const SettingDescriptions: Record<string, SettingDescription> = {
  "typing.delay": {
    title: "Typing delay",
    description: "Time to wait between characters typed by text macros. Increase it if applications drop characters.",
    unit: "ms",
  },
}

export function getSettingDescription(key: string): SettingDescription {
  return SettingDescriptions[key] ?? { title: key, description: "" }
}
