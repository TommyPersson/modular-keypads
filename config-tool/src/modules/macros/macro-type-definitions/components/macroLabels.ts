import { consumerControlCodes } from "@src/modules/macros/data/consumerControlCodes"
import { keyboadKeyCodes } from "@src/modules/macros/data"
import { mouseButtons } from "@src/modules/macros/data/mouseButtons"
import { systemControlCodes } from "@src/modules/macros/data/systemControlCodes"
import type { MacroDefinition, SequenceStep, ShortcutMacroDefinition } from "@src/modules/macros/models"

// The texts of the macro definition chips, shared with the macro filter so that it finds what the chips show

export function modifiersLabel(modifiers: number[]): string | null {
  if (modifiers.length === 0) {
    return null
  }

  return modifiers
    .map(hidCode => keyboadKeyCodes.byHidCode[hidCode])
    .map(it => it.hidDescription)
    .join(" + ")
}

export function keyCodeLabel(hidCode: number): { description: string, code: string } {
  const keyCode = keyboadKeyCodes.byHidCode[hidCode]
  return {
    description: keyCode?.hidDescription ?? "",
    code: `0x${hidCode.toString(16).padStart(2, "0")}`,
  }
}

export function shortcutHoldLabel(macro: ShortcutMacroDefinition): string | null {
  if (!macro.holdModifiers) {
    return null
  }

  return macro.holdTimeMs > 0 ? `Hold ${macro.holdTimeMs} ms` : "Hold"
}

export function consumerControlLabel(usageId: number): { name: string, code: string } | null {
  const code = consumerControlCodes.byUsageId[usageId]
  if (!code) {
    return null
  }

  return { name: code.usageName, code: `0x${usageId.toString(16)}` }
}

export function systemControlLabel(code: number): string | null {
  return systemControlCodes.byCode[code]?.description ?? null
}

export function mouseButtonLabel(buttons: number): string | null {
  const clickedButtons = mouseButtons
    .filter(({ button }) => (buttons & button) !== 0)
    .map(({ name }) => name)
  if (clickedButtons.length === 0) {
    return null
  }

  return `Click ${clickedButtons.join(" + ")}`
}

export function mouseMovementLabel(x: number, y: number): string | null {
  if (x === 0 && y === 0) {
    return null
  }

  return `Move ${x}, ${y}`
}

export function mouseScrollLabel(wheel: number, pan: number): string | null {
  const directions: string[] = []
  if (wheel !== 0) {
    directions.push(`${wheel > 0 ? "up" : "down"} ${Math.abs(wheel)}`)
  }
  if (pan !== 0) {
    directions.push(`${pan > 0 ? "right" : "left"} ${Math.abs(pan)}`)
  }
  if (directions.length === 0) {
    return null
  }

  return `Scroll ${directions.join(", ")}`
}

const NOTE_NAMES = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"]

// With middle C (60) as C4, the most common numbering, though some programs call it C3
export function midiNoteName(note: number): string {
  return `${NOTE_NAMES[note % 12]}${Math.floor(note / 12) - 1}`
}

// The number first, as apps controlled with MIDI show and map notes by number
export function midiNoteLabel(note: number): string {
  return `Note ${note} (${midiNoteName(note)})`
}

export function midiRelativeControlChangeLabel(controller: number, amount: number): string {
  return `CC ${controller} ${amount >= 0 ? "+" : ""}${amount}`
}

export function midiChannelLabel(channel: number): string {
  return `Ch ${channel}`
}

// The name of the macro of a sequence step, or a note when there's none, such as after it was deleted
export function sequenceStepLabel(step: SequenceStep, macros: MacroDefinition[]): string {
  const macro = macros.find(it => it.id === step.macroId)
  return macro?.name ?? "Missing macro"
}

// Short, as it's shown in small print below the arrow to the step, e.g. 500ms, 2s, or 1.5s
export function sequenceDelayLabel(delayMs: number): string | null {
  if (delayMs <= 0) {
    return null
  }

  return delayMs < 1000 ? `${delayMs}ms` : `${parseFloat((delayMs / 1000).toFixed(2))}s`
}

// Leaves out the labels that aren't shown
export function texts(...labels: (string | null | undefined)[]): string[] {
  return labels.filter(it => !!it) as string[]
}
