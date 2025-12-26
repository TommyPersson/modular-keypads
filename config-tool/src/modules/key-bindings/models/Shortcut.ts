export type Shortcut = {
  modifiers: number[]
  hidCode: number
}

export function createModifierFlags(modifiers: number[]): number {
  return (
    setFlag(modifiers.includes(0xe0), 0) | // Control (L)
    setFlag(modifiers.includes(0xe1), 1) | // Shift (L)
    setFlag(modifiers.includes(0xe2), 2) | // Alt (L)
    setFlag(modifiers.includes(0xe3), 3) | // Meta (L)
    setFlag(modifiers.includes(0xe4), 4) | // Control (R)
    setFlag(modifiers.includes(0xe5), 5) | // Shift (R)
    setFlag(modifiers.includes(0xe6), 6) | // Alt (R)
    setFlag(modifiers.includes(0xe7), 7)   // Meta (R)
  )
}

export function parseModifierFlags(modifiers: number): number[] {
  const result = []

  if (checkFlag(modifiers, 0)) result.push(0xe0) // Control (L)
  if (checkFlag(modifiers, 1)) result.push(0xe1) // Shift (L)
  if (checkFlag(modifiers, 2)) result.push(0xe2) // Alt (L)
  if (checkFlag(modifiers, 3)) result.push(0xe3) // Meta (L)
  if (checkFlag(modifiers, 4)) result.push(0xe4) // Control (R)
  if (checkFlag(modifiers, 5)) result.push(0xe5) // Shift (R)
  if (checkFlag(modifiers, 6)) result.push(0xe6) // Alt (R)
  if (checkFlag(modifiers, 7)) result.push(0xe7) // Meta (R)

  return result
}

function checkFlag(flags: number, bitNumber: number) {
  return flags & (1 << bitNumber)
}

function setFlag(value: boolean, bitNumber: number): number {
  if (!value) {
    return 0
  }

  return (1 << bitNumber)
}