// Names of the control change controllers in the MIDI 1.0 specification, shortened to fit next to the controller
// number. Controllers 120 to 127 are channel mode messages, which macros can't send.

const MSB_CONTROLLER_NAMES: { [controller: number]: string } = {
  0: "Bank Select",
  1: "Modulation",
  2: "Breath",
  4: "Foot",
  5: "Portamento Time",
  6: "Data Entry",
  7: "Volume",
  8: "Balance",
  10: "Pan",
  11: "Expression",
  12: "Effect 1",
  13: "Effect 2",
  16: "General Purpose 1",
  17: "General Purpose 2",
  18: "General Purpose 3",
  19: "General Purpose 4",
}

const OTHER_CONTROLLER_NAMES: { [controller: number]: string } = {
  64: "Sustain",
  65: "Portamento",
  66: "Sostenuto",
  67: "Soft Pedal",
  68: "Legato",
  69: "Hold 2",
  70: "Sound Variation",
  71: "Timbre",
  72: "Release Time",
  73: "Attack Time",
  74: "Brightness",
  75: "Sound Controller 6",
  76: "Sound Controller 7",
  77: "Sound Controller 8",
  78: "Sound Controller 9",
  79: "Sound Controller 10",
  80: "General Purpose 5",
  81: "General Purpose 6",
  82: "General Purpose 7",
  83: "General Purpose 8",
  84: "Portamento Control",
  88: "Velocity Prefix",
  91: "Reverb",
  92: "Tremolo",
  93: "Chorus",
  94: "Detune",
  95: "Phaser",
  96: "Data Increment",
  97: "Data Decrement",
  98: "NRPN LSB",
  99: "NRPN MSB",
  100: "RPN LSB",
  101: "RPN MSB",
}

// Controllers 32 to 63 are the low bytes of 0 to 31, for 14-bit values. Controllers without a standard use are
// "Undefined", which makes them the safest choice for controlling apps, as nothing else interprets them.
export function midiControllerName(controller: number): string {
  if (controller >= 32 && controller <= 63) {
    const msbName = MSB_CONTROLLER_NAMES[controller - 32]
    return msbName ? `${msbName} LSB` : "Undefined"
  }

  return MSB_CONTROLLER_NAMES[controller] ?? OTHER_CONTROLLER_NAMES[controller] ?? "Undefined"
}
