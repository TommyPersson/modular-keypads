
export interface DeviceCommand<TResult> {
  type: string
  arguments: string[]
  parseResponse(lines: string[]): TResult
}