import { Deferred } from "./Deferred"

export class Mutex {

  private _lock: Deferred<void> | null = null

  async acquire(timeoutMs: number): Promise<void> {
    const timeoutDeferred = new Deferred()

    const lock = this._lock;
    if (lock) {
      setTimeout(() => {
        if (!lock.isResolved) {
          timeoutDeferred.reject(new Error("timeout"))
        }
      }, timeoutMs)

      await Promise.race([lock.promise, timeoutDeferred.promise])
    }

    this._lock = new Deferred<void>()
  }

  release(): void {
    this._lock?.resolve()
    this._lock = null
  }

  get isLocked(): boolean {
    return this._lock !== null
  }
}