// The data arguments of macros are "key=value" fields, so that fields can be added and removed without breaking other
// firmware or config tool versions: keys that aren't known are skipped, and missing keys take their defaults.
export class DeviceFields {
  private constructor(
    private readonly values: Map<string, string>,
  ) {
  }

  static parse(dataArgs: string[]): DeviceFields | null {
    // Arguments without keys can't be told apart from each other
    if (!dataArgs.every(arg => arg.includes("="))) {
      return null
    }

    const values = new Map<string, string>()
    for (const arg of dataArgs) {
      const separator = arg.indexOf("=")
      const key = arg.substring(0, separator)
      if (!values.has(key)) {
        values.set(key, arg.substring(separator + 1))
      }
    }
    return new DeviceFields(values)
  }

  get(key: string): string | undefined {
    return this.values.get(key)
  }

  number(key: string, defaultValue: number, radix: number = 10): number {
    const value = parseInt(this.values.get(key) ?? "", radix)
    return Number.isNaN(value) ? defaultValue : value
  }

  flag(key: string, defaultValue: boolean = false): boolean {
    const value = this.values.get(key)
    return value === undefined ? defaultValue : (parseInt(value, 10) || 0) !== 0
  }
}

export function serializeDeviceFields(fields: Record<string, string | number | boolean>): string[] {
  return Object.entries(fields).map(([key, value]) => {
    const serialized = typeof value === "boolean" ? (value ? "1" : "0") : `${value}`
    return `${key}=${serialized}`
  })
}

export function deviceHex(value: number, digits: number): string {
  return `0x${value.toString(16).padStart(digits, "0")}`
}
