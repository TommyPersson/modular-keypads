export async function* readLines(readable: ReadableStream, abortSignal: AbortSignal): AsyncIterable<string> {
  const textDecoder = new TextDecoderStream()
  const readableStreamClosed = readable.pipeTo(textDecoder.writable, { signal: abortSignal })
  const reader = textDecoder.readable.getReader()

  let currentLine = ""

  try {
    while (!abortSignal.aborted) {
      const read = await reader.read()
      const text = read.value ?? ""
      for (const char of text) {
        if (char == "\n") {
          yield currentLine
          currentLine = ""
        } else {
          currentLine += char
        }
      }
      if (read.done) {
        yield currentLine
        break
      }
    }
  } catch (e) {
    console.error("readLines error", e)
  } finally {
    // noop
  }

  reader.releaseLock()
  await readableStreamClosed.catch(() => {
  })
}