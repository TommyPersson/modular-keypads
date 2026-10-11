import AddOutlinedIcon from "@mui/icons-material/AddOutlined"
import ClearOutlinedIcon from "@mui/icons-material/ClearOutlined"
import EditOutlinedIcon from "@mui/icons-material/EditOutlined"
import FolderOutlinedIcon from "@mui/icons-material/FolderOutlined"
import KeyboardCommandKeyOutlinedIcon from "@mui/icons-material/KeyboardCommandKeyOutlined"
import SearchOutlinedIcon from "@mui/icons-material/SearchOutlined"
import {
  Box,
  Button,
  CardHeader,
  Chip,
  IconButton,
  InputAdornment,
  Stack,
  Table,
  TableBody,
  TableCell, TableContainer,
  TableHead,
  TableRow,
  TextField,
  Tooltip
} from "@mui/material"
import { CommandButton, EmptyTableRow } from "@src/modules/common/components"
import { useExpanderState } from "@src/modules/common/hooks/expanderState"
import { DeleteMacroCommand } from "@src/modules/macros/commands"
import { useStoredMacros } from "@src/modules/macros/hooks"
import { macroTypeDefinitions } from "@src/modules/macros/macro-type-definitions"
import { type MacroDefinition } from "@src/modules/macros/models"
import { MacroTypeVisualization } from "@src/modules/macros/ui"
import { EditMacroDialog } from "./EditMacroDialog"
import { useCallback, useMemo, useState } from "react"

export const MacrosToolPaneContent = () => {
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

  const [filter, setFilter] = useState("")
  const filteredMacros = useMemo(
    () => filterMacros(storedMacros.macrosByDirectoryList, filter),
    [storedMacros.macrosByDirectoryList, filter],
  )

  return (
    // Fills the pane, so that only the macro list scrolls rather than the whole pane
    <Stack sx={{ height: "100%" }}>
      <CardHeader
        title={
          <Stack spacing={1} direction={"row"} alignItems={"center"}>
            <KeyboardCommandKeyOutlinedIcon />
            <span>Macros</span>
          </Stack>
        }
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
      <Box sx={{ px: 2, pb: 1 }}>
        <TextField
          size={"small"}
          fullWidth
          placeholder={"Filter by name, directory or definition"}
          value={filter}
          onChange={e => setFilter(e.target.value)}
          slotProps={{
            input: {
              startAdornment: <InputAdornment position={"start"}><SearchOutlinedIcon /></InputAdornment>,
              endAdornment: filter && (
                <InputAdornment position={"end"}>
                  <IconButton size={"small"} onClick={() => setFilter("")}>
                    <ClearOutlinedIcon fontSize={"small"} />
                  </IconButton>
                </InputAdornment>
              ),
            },
          }}
        />
      </Box>
      <ExistingMacrosCardContent
        macros={filteredMacros}
        isFiltered={filter.trim().length > 0}
        hasMacros={storedMacros.allMacros.length > 0}
        onEditClick={handleEditClicked}
      />
    </Stack>
  )
}

// Keeps the macros that match all words of the filter, along with their directories. The words can be found in the
// name, the directory or the definition as shown in the table, e.g. a modifier or a key code.
function filterMacros(macrosByDirectory: [string, MacroDefinition[]][], filter: string): [string, MacroDefinition[]][] {
  const words = filter.trim().toLocaleLowerCase().split(/\s+/).filter(it => it.length > 0)
  if (words.length === 0) {
    return macrosByDirectory
  }

  const matches = (macro: MacroDefinition) => {
    const searchTexts = macroTypeDefinitions.byType[macro.type]?.searchTexts(macro) ?? []
    const text = [macro.name, macro.directory ?? "", ...searchTexts].join("\n").toLocaleLowerCase()
    return words.every(word => text.includes(word))
  }

  return macrosByDirectory
    .map(([directory, macros]): [string, MacroDefinition[]] => [directory, macros.filter(matches)])
    .filter(([, macros]) => macros.length > 0)
}

const ExistingMacrosCardContent = (props: {
  macros: [string, MacroDefinition[]][]
  isFiltered: boolean
  hasMacros: boolean
  onEditClick: (macro: MacroDefinition) => void
}) => {

  const { macros, isFiltered, hasMacros, onEditClick } = props

  return (
    <>
      {/* Clear of the bottom of the pane, so that the scrollbar doesn't run into its rounded corners */}
      <Box sx={{ flex: 1, minHeight: 0, display: "flex", flexDirection: "column", mb: 1.5 }}>
        <TableContainer sx={{ flex: 1, minHeight: 0, overflow: "auto" }}>
          <Table size={"small"} stickyHeader>
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
                  {hasMacros && isFiltered ? "No macros match the filter." : "There are no recorded macros."}
                </EmptyTableRow>
              )}
              {macros.map(([directory, macros]) => (
                <MacroDefinitionRows
                  key={directory}
                  directory={directory}
                  macros={macros}
                  forceExpanded={isFiltered}
                  onEditClick={onEditClick}
                />
              ))}
            </TableBody>
          </Table>
        </TableContainer>
      </Box>
    </>
  )
}

const MacroDefinitionRows = (props: {
  directory: string,
  macros: MacroDefinition[],
  // While filtering, so that the matches in collapsed directories are shown too
  forceExpanded: boolean,
  onEditClick: (macro: MacroDefinition) => void
}) => {
  const { directory, macros, forceExpanded, onEditClick } = props

  const expanderState = useExpanderState(true)
  const isExpanded = forceExpanded || expanderState.isExpanded

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

      {isExpanded ? macros.map(macro =>
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

  return (
    <TableRow hover sx={rowActionsOnHover}>
      <TableCell style={{ whiteSpace: "nowrap", verticalAlign }}>
        <Stack direction={"row"} alignItems={"center"} gap={1} style={{ marginLeft: 10 }}>
          <MacroTypeVisualization type={macro.type} /> {macro.name}
        </Stack>
      </TableCell>
      <TableCell style={{ maxWidth: 0, overflow: "hidden" }}>{definitionCellContent}</TableCell>
      <TableCell style={{ verticalAlign }} align={"right"}>
        <Stack direction={"row"} className={"row-actions"}>
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

// Hidden rather than removed, so that the rows don't change their layout. Also shown while focused by keyboard.
const rowActionsOnHover = {
  "& .row-actions": { visibility: "hidden" },
  "&:hover .row-actions, &:focus-within .row-actions": { visibility: "visible" },
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
