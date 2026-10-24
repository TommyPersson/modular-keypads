import type { DeviceFacade } from "@src/modules/device/facade/DeviceFacade"
import { BehaviorSubject, type Observable } from "rxjs"
import { type FirmwareUpdateProgress, updateFirmware } from "./updateFirmware"
import { validateFirmwareImage } from "./validateFirmwareImage"

export type FirmwareUpdateState = {
  isOpen: boolean
  deviceName: string
  file: File | null
  // Why the selected file can't be used, if it can't
  fileError: string | null
  progress: FirmwareUpdateProgress | null
  error: string | null
}

const initialState: FirmwareUpdateState = {
  isOpen: false,
  deviceName: "",
  file: null,
  fileError: null,
  progress: null,
  error: null,
}

export function isFirmwareUpdateRunning(state: FirmwareUpdateState): boolean {
  const { progress, error } = state
  return progress !== null && progress.phase !== "done" && error === null
}

/**
 * Holds the state of the firmware update dialog outside of React, as the device, and with it the components that
 * open the dialog, disconnect while it restarts into the new firmware.
 */
export class FirmwareUpdateController {
  private readonly stateSubject = new BehaviorSubject<FirmwareUpdateState>(initialState)
  private abortController: AbortController | null = null

  get state$(): Observable<FirmwareUpdateState> {
    return this.stateSubject
  }

  get state(): FirmwareUpdateState {
    return this.stateSubject.value
  }

  open(deviceName: string) {
    if (isFirmwareUpdateRunning(this.state)) {
      return
    }

    this.stateSubject.next({ ...initialState, isOpen: true, deviceName })
  }

  close() {
    if (isFirmwareUpdateRunning(this.state)) {
      return
    }

    this.stateSubject.next(initialState)
  }

  selectFile(file: File | null) {
    if (isFirmwareUpdateRunning(this.state)) {
      return
    }

    this.update({ file, fileError: null, progress: null, error: null })

    if (file) {
      this.checkFile(file).then()
    }
  }

  // Checked when selected, so that a wrong file is pointed out before the user tries to update with it
  private async checkFile(file: File) {
    let fileError: string | null = null
    try {
      await validateFirmwareImage(new Uint8Array(await file.arrayBuffer()))
    } catch (e) {
      fileError = (e as Error).message
    }

    // Unless another file has been selected in the meantime
    if (this.state.file === file) {
      this.update({ fileError })
    }
  }

  async start(facade: DeviceFacade) {
    const { file, fileError } = this.state
    if (!file || fileError || this.abortController || isFirmwareUpdateRunning(this.state)) {
      return
    }

    const abortController = new AbortController()
    this.abortController = abortController

    // The progress is reported once the image has been checked, so a file that isn't an image only shows the error
    this.update({ progress: null, error: null })

    try {
      const image = new Uint8Array(await file.arrayBuffer())
      await updateFirmware({
        facade,
        image,
        onProgress: progress => this.update({ progress }),
        abortSignal: abortController.signal,
      })
    } catch (e) {
      this.update({ error: abortController.signal.aborted ? "The update was cancelled" : (e as Error).message })
    } finally {
      this.abortController = null
    }
  }

  // Only the upload can be cancelled, as the device restarts on its own once it has the whole image
  cancel() {
    this.abortController?.abort()
  }

  private update(changes: Partial<FirmwareUpdateState>) {
    this.stateSubject.next({ ...this.stateSubject.value, ...changes })
  }
}

export const globalFirmwareUpdateController = new FirmwareUpdateController()
