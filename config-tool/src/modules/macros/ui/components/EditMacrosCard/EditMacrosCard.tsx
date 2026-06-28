import AddOutlinedIcon from "@mui/icons-material/AddOutlined"
import EditOutlinedIcon from "@mui/icons-material/EditOutlined"
import FolderOutlinedIcon from "@mui/icons-material/FolderOutlined"
import {
  Button,
  Card,
  CardContent,
  CardHeader,
  Chip,
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
import { useExpanderState } from "@src/modules/common/hooks/expanderState"
import { DeleteMacroCommand } from "@src/modules/macros/commands"
import { useStoredMacros } from "@src/modules/macros/hooks"
import { macroTypeDefinitions } from "@src/modules/macros/macro-type-definitions"
import { type MacroDefinition, MacroDefinitionType } from "@src/modules/macros/models"
import { EditMacroDialog } from "@src/modules/macros/ui/components/EditMacrosCard/EditMacroDialog"
import { useCallback, useMemo, useState } from "react"

export const EditMacrosCard = () => {
  const storedMacros = useStoredMacros()

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
      <EditMacroDialog
        macro={macroBeingEdited}
        isOpen={isEditDialogOpen}
        onClose={handleEditDialogClosed}
      />
      <ExistingMacrosCardContent
        macros={storedMacros.macrosByDirectoryList}
        onEditClick={handleEditClicked}
      />
    </Card>
  )
}

const ExistingMacrosCardContent = (props: {
  macros: [string, MacroDefinition[]][]
  onEditClick: (macro: MacroDefinition) => void
}) => {

  const { macros, onEditClick } = props

  return (
    <>
      <CardContent style={{ paddingLeft: 0, paddingRight: 0 }}>
        <Table size={"small"}>
          <TableHead>
            <TableRow>
              <TableCell style={{ width: 0 }}>Name</TableCell>
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
            {macros.map(([directory, macros]) => (
              <MacroDefinitionRows directory={directory} macros={macros} onEditClick={onEditClick} />
            ))}
          </TableBody>
        </Table>
      </CardContent>
    </>
  )
}

const MacroDefinitionRows = (props: {
  directory: string,
  macros: MacroDefinition[],
  onEditClick: (macro: MacroDefinition) => void
}) => {
  const { directory, macros, onEditClick } = props

  const expanderState = useExpanderState(true)

  return (
    <>
      <TableRow hover onClick={expanderState.toggle}>
        <TableCell colSpan={5}>
          <Stack direction={"row"} alignItems={"center"} gap={1}>
            <FolderOutlinedIcon />
            <strong>{directory}</strong>
            <Chip size={"small"} label={macros.length} />
            <IconButton size={"small"}>{expanderState.icon}</IconButton>
          </Stack>
        </TableCell>
      </TableRow>

      {expanderState.isExpanded ? macros.map(macro =>
        <MacroDefinitionRow
          key={macro.id}
          macro={macro}
          onEditClick={onEditClick}
        />
      ) : null}
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

  const nameWithoutDirectory = useMemo(() => {
    const parts = macro.name.split("/")
    return parts[parts.length - 1]
  }, [macro])

  return (
    <TableRow hover>
      <TableCell style={{ whiteSpace: "nowrap", verticalAlign }}>
        <Stack direction={"row"} alignItems={"center"} gap={1} style={{ marginLeft: 10 }}>
          <MacroTypeVisualization type={macro.type} /> {nameWithoutDirectory}
        </Stack>
      </TableCell>
      <TableCell style={{ maxWidth: 0, overflow: "hidden" }}>{definitionCellContent}</TableCell>
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
  const typeDefinition = macroTypeDefinitions.byType[macro.type]
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

  const typeDefinition = macroTypeDefinitions.byType[type]
  if (typeDefinition) {
    return <typeDefinition.typeVisualizationComponent />
  }

  return null
}
