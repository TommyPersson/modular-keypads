// Macros are stored with their folder and name as one path, of URI encoded segments after a leading slash:
// "/Folder/Sub%20folder/My%20Macro". Encoding the segments rather than the whole path keeps slashes in names apart
// from the ones between folders, and keeps the colons and commas of the device's lines out of both.
export function encodeMacroPath(directory: string | null, name: string): string {
  const segments = [...(directory ? directory.split("/") : []), name]
  return `/${segments.map(encodeURIComponent).join("/")}`
}

export function decodeMacroPath(encodedPath: string): { directory: string | null, name: string } {
  const segments = encodedPath.startsWith("/")
    ? encodedPath.substring(1).split("/").map(decodeURIComponent)
    : decodeLegacyMacroPath(encodedPath)

  const directorySegments = segments.slice(0, segments.length - 1)

  return {
    directory: directorySegments.length > 0 ? directorySegments.join("/") : null,
    name: segments[segments.length - 1],
  }
}

// Paths saved before they had a leading slash are URI encoded twice instead: the name, and then the whole path
function decodeLegacyMacroPath(encodedPath: string): string[] {
  const segments = decodeURIComponent(encodedPath).split("/")
  segments[segments.length - 1] = decodeURIComponent(segments[segments.length - 1])
  return segments
}
