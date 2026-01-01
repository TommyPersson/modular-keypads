import AddOutlinedIcon from "@mui/icons-material/AddOutlined"
import EditOutlinedIcon from "@mui/icons-material/EditOutlined"

import {
  Button,
  Card,
  CardContent,
  CardHeader,
  IconButton,
  Stack,
  Table,
  TableBody,
  TableCell,
  TableHead,
  TableRow,
  Tooltip
} from "@mui/material"
import { CommandButton, EmptyTableRow } from "@src/modules/common/components"
import { DeleteMacroCommand } from "@src/modules/key-bindings/commands"
import { useStoredMacrosQuery } from "@src/modules/key-bindings/hooks"
import { type MacroDefinition, MacroDefinitionType } from "@src/modules/key-bindings/models"
import { EditMacroDialog } from "@src/modules/key-bindings/ui/components/EditMacrosCard/EditMacroDialog"
import { macroTypeDefinitionsByType } from "@src/modules/macro-types"
import { useCallback, useState } from "react"

export const EditMacrosCard = () => {
  const storedMacros = useStoredMacrosQuery().data ?? []

  const [isEditDialogOpen, setIsEditDialogOpen] = useState<boolean>(false)
  const [macroBeingEdited, setMacroBeingEdited] = useState<MacroDefinition | null>(null)

  const handleOpenEditDialogClick = useCallback(() => {
    setIsEditDialogOpen(true)
  }, [setIsEditDialogOpen])

  const handleEditDialogClosed = useCallback(() => {
    setIsEditDialogOpen(false)
    setMacroBeingEdited(null)
  }, [setIsEditDialogOpen, setMacroBeingEdited])

  const handleEditClicked = useCallback((macro: MacroDefinition) => {
    setMacroBeingEdited(macro)
    setIsEditDialogOpen(true)
  }, [setMacroBeingEdited, setIsEditDialogOpen])

  return (
    <Card>
      <CardHeader
        title={"Macros"}
        subheader={"Create macros to be able to bind them to keys"}
        action={
          <>
            <Button startIcon={<AddOutlinedIcon />} onClick={handleOpenEditDialogClick}>Create</Button>
          </>
        }
      />
      <EditMacroDialog macro={macroBeingEdited} isOpen={isEditDialogOpen} onClose={handleEditDialogClosed} />
      <ExistingMacrosCardContent macros={storedMacros} onEditClick={handleEditClicked} />
    </Card>
  )
}

const ExistingMacrosCardContent = (props: {
  macros: MacroDefinition[]
  onEditClick: (macro: MacroDefinition) => void
}) => {

  const { macros, onEditClick } = props

  return (
    <>
      <CardHeader title={"Existing Macros"} />
      <CardContent style={{ paddingLeft: 0, paddingRight: 0 }}>
        <Table size={"small"}>
          <TableHead>
            <TableRow>
              <TableCell style={{ width: 0 }}>ID</TableCell>
              <TableCell style={{ width: 0 }}>Name</TableCell>
              <TableCell style={{ width: 0 }}>Type</TableCell>
              <TableCell>Definition</TableCell>
              <TableCell style={{ width: 0 }} align={"right"}>Actions</TableCell>
            </TableRow>
          </TableHead>
          <TableBody>
            {macros.length === 0 && (
              <EmptyTableRow>
                There are no recorded macros.
              </EmptyTableRow>
            )}
            {macros.map(macro =>
              <MacroDefinitionRow
                key={macro.name}
                macro={macro}
                onEditClick={onEditClick}
              />
            )}
          </TableBody>
        </Table>
      </CardContent>
    </>
  )
}

const MacroDefinitionRow = (props: {
  macro: MacroDefinition
  onEditClick: (macro: MacroDefinition) => void
}) => {
  const { macro, onEditClick } = props

  const definitionCellContent = createMacroDefinitionCellContent(macro)

  const verticalAlign = isSequenceMacro(macro) ? "top" : undefined

  const handleEditIconClick = useCallback(() => {
    onEditClick(macro)
  }, [macro, onEditClick])

  return (
    <TableRow hover>
      <TableCell style={{ whiteSpace: "nowrap", verticalAlign }}>{macro.id}</TableCell>
      <TableCell style={{ whiteSpace: "nowrap", verticalAlign }}>{macro.name}</TableCell>
      <TableCell style={{ whiteSpace: "nowrap", verticalAlign }}>
        <MacroTypeVisualization type={macro.type} />
      </TableCell>
      <TableCell>{definitionCellContent}</TableCell>
      <TableCell style={{ verticalAlign }} align={"right"}>
        <Stack direction={"row"}>
          <Tooltip title={"Edit Macro"}>
            <IconButton onClick={handleEditIconClick} size={"small"}>
              <EditOutlinedIcon fontSize={"small"} />
            </IconButton>
          </Tooltip>
          <CommandButton
            command={DeleteMacroCommand}
            args={{ macro }}
            size={"small"}
            iconOnly
          />
        </Stack>
      </TableCell>
    </TableRow>
  )
}

function createMacroDefinitionCellContent(macro: MacroDefinition) {
  const typeDefinition = macroTypeDefinitionsByType[macro.type]
  if (typeDefinition) {
    return <typeDefinition.macroTableCellComponent macro={macro} />
  }

  return null
}

function isSequenceMacro(macro: MacroDefinition) {
  switch (macro.type) { // TODO move to MacroTypeDefinition when needed
    default:
      return false
  }
}

const MacroTypeVisualization = (props: {
  type: MacroDefinitionType
}) => {
  const { type } = props

  const typeDefinition = macroTypeDefinitionsByType[type]
  if (typeDefinition) {
    return <typeDefinition.typeVisualizationComponent />
  }

  return null
}
