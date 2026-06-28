
export class FakeSerialPort implements SerialPort {
  onconnect: (this: SerialPort, ev: Event) => void = null!

  ondisconnect: (this: SerialPort, ev: Event) => void = null!

  connected: boolean = false

  readable: ReadableStream<Uint8Array<ArrayBufferLike>> = null!

  writable: WritableStream<Uint8Array<ArrayBufferLike>> = null!

  async open(_options: SerialOptions): Promise<void> {
    this.connected = true
    return
  }

  async setSignals(_signals: SerialOutputSignals): Promise<void> {
    return
  }

  async getSignals(): Promise<SerialInputSignals> {
    throw new Error("Method not implemented.")
  }

  getInfo(): SerialPortInfo {
    throw new Error("Method not implemented.")
  }

  async close(): Promise<void> {
    throw new Error("Method not implemented.")
  }

  async forget(): Promise<void> {
    throw new Error("Method not implemented.")
  }

  addEventListener(_type: unknown, _listener: unknown, _options?: unknown): void {
    throw new Error("Method not implemented.")
  }

  removeEventListener(_type: unknown, _callback: unknown, _options?: unknown): void {
    throw new Error("Method not implemented.")
  }

  dispatchEvent(_event: Event): boolean {
    throw new Error("Method not implemented.")
  }
}

export class FakeSerial implements Serial {
  onconnect: (this: this, ev: Event) => void = null!
  ondisconnect: (this: this, ev: Event) => void = null!

  async getPorts(): Promise<SerialPort[]> {
    return [];
  }

  async requestPort(_options?: SerialPortRequestOptions): Promise<SerialPort> {
    return new FakeSerialPort()
  }

  addEventListener(_type: unknown, _listener: unknown, _options?: unknown): void {
    throw new Error("Method not implemented.")
  }

  removeEventListener(_type: unknown, _callback: unknown, _options?: unknown): void {
    throw new Error("Method not implemented.")
  }

  dispatchEvent(_event: Event): boolean {
    throw new Error("Method not implemented.")
  }
}