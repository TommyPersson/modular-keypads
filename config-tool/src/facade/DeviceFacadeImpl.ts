import { BehaviorSubject, Observable, Subject } from "rxjs"
import type { DeviceFacade, LogMessage } from "./DeviceFacade"

export class DeviceFacadeImpl implements DeviceFacade {

  private logsSubject = new Subject<LogMessage>()
  private isConnectedSubject = new BehaviorSubject<boolean>(false)

  private disconnectAbortController: AbortController | null = null

  private port: SerialPort | null = null

  constructor() {
    // Causes the WebSerial stuff to be initialized
    navigator.serial.getPorts().then()
  }

  async connect(): Promise<void> {
    if (this.port?.connected) {
      return
    }


    try {
      await navigator.serial.getPorts()

      const abortController = new AbortController()
      this.disconnectAbortController = abortController

      this.port = await navigator.serial.requestPort({})
      this.port.open({ baudRate: 115200 }).then(async () => {
        this.isConnectedSubject.next(true)
        try {
          if (this.port?.readable) {
            for await (const line of readLines(this.port?.readable, abortController.signal)) {
              if (!this.port.connected) {
                break
              }

              if (!line.startsWith("#")) {
                continue
              }

              this.logsSubject.next({ direction: "to-host", message: line })
              console.log("line: ", line)
            }
          }
        } catch (e) {
          console.error(e)
        } finally {
          await this.close()
        }
      })
    } catch(e) {
      console.error(e)
      await this.close()
    }
  }

  async disconnect(): Promise<void> {
    this.disconnectAbortController?.abort("disconnecting")
  }

  async performPing() {
    // TODO
  }

  private async close() {
    console.log("closing..")
    try {
      await this.port?.close()
    } catch (e) {
      console.error("unable to close", e)
    }

    this.port = null
    this.disconnectAbortController = null
    this.isConnectedSubject.next(false)
  }

  get $logs(): Observable<LogMessage> {
    return this.logsSubject
  }

  get $isConnected(): Observable<boolean> {
    return this.isConnectedSubject
  }
}

async function* readLines(readable: ReadableStream, abortSignal: AbortSignal): AsyncIterable<string> {
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
  await readableStreamClosed.catch(() => {})

  console.log("readLines exiting")
}