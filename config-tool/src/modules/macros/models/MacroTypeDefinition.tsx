import { type MacroDefinition, MacroDefinitionType } from "@src/modules/macros/models"
import * as React from "react"

export type MacroTypeDefinition<TMacroDefinition extends MacroDefinition> = {
  type: MacroDefinitionType

  numericCode: number

  displayName: string

  description: any

  handles(type: MacroDefinitionType): boolean

  createDefaultMacroDefinition(name: string): TMacroDefinition

  parseDeviceResponse(id: number, name: string, dataArgs: string[]): TMacroDefinition

  serializeDeviceArguments(definition: TMacroDefinition): string[]

  editorInformationComponent: React.ElementType<unknown>

  editorComponent: React.ElementType<{
    value: TMacroDefinition;
    onChange: (value: TMacroDefinition) => void;
  }>

  typeVisualizationComponent: React.ElementType<unknown>

  macroTableCellComponent: React.ElementType<{ macro: TMacroDefinition }>
}