
// The longest command line the device reads, without the newline. Longer lines are dropped without a response.
export const MaxCommandLineLength = 1024

// Command IDs go up to this before starting over
export const MaxCommandId = 999

export function formatCommandLine(commandId: number, type: string, args: string[]): string {
  return args.length > 0 ? `${commandId}:${type}:${args.join(",")}` : `${commandId}:${type}`
}

// The length of the command line in bytes, with the longest command ID, as that is what the device counts
export function commandLineLength(command: DeviceCommand<unknown>): number {
  return new TextEncoder().encode(formatCommandLine(MaxCommandId, command.type, command.arguments)).length
}

export abstract class DeviceCommand<TResult> {
  abstract type: string

  get arguments(): string[] { return [] }

  // How long to wait for the response, when the command is known to take longer than most
  get timeoutMs(): number | undefined { return undefined }

  abstract parseResponse(lines: string[]): TResult
}