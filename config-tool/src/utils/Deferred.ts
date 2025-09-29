export class Deferred<T> {

  private readonly resolvers: PromiseWithResolvers<T>

  private _isResolved: boolean = false
  private _isRejected: boolean = false

  constructor() {
    this.resolvers = Promise.withResolvers<T>()
  }

  get promise(): Promise<T> {
    return this.resolvers.promise
  }

  get isCompleted(): boolean {
    return this._isRejected || this._isResolved
  }

  get isResolved(): boolean {
    return this._isResolved
  }

  get isRejected(): boolean {
    return this._isRejected
  }

  resolve(value: T) {
    this.resolvers.resolve(value)
  }

  reject(reason: Error) {
    this.resolvers.reject(reason)
  }
}

