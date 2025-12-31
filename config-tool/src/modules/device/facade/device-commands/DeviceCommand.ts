
export abstract class DeviceCommand<TResult> {
  abstract type: string

  get arguments(): string[] { return [] }

  abstract parseResponse(lines: string[]): TResult
}