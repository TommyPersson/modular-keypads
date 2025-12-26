export type Shortcut = {
  modifiers: number[]
  hidCode: number
}

export function createModifierFlags(modifiers: number[]): number {
  return (
    0 << (modifiers.includes(0xe0) ? 1 : 0) | // Control (L)
    1 << (modifiers.includes(0xe1) ? 1 : 0) | // Shift (L)
    2 << (modifiers.includes(0xe2) ? 1 : 0) | // Alt (L)
    3 << (modifiers.includes(0xe3) ? 1 : 0) | // Meta (L)
    4 << (modifiers.includes(0xe4) ? 1 : 0) | // Control (R)
    5 << (modifiers.includes(0xe5) ? 1 : 0) | // Shift (R)
    6 << (modifiers.includes(0xe6) ? 1 : 0) | // Alt (R)
    7 << (modifiers.includes(0xe7) ? 1 : 0)   // Meta (R)
  )
}