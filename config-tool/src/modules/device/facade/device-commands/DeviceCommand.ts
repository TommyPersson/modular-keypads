
export abstract class DeviceCommand<TResult> {
  abstract type: string

  get arguments(): string[] { return [] }

  // How long to wait for the response, when the command is known to take longer than most
  get timeoutMs(): number | undefined { return undefined }

  abstract parseResponse(lines: string[]): TResult
}