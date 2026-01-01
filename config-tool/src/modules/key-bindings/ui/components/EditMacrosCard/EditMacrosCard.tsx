import AddOutlinedIcon from "@mui/icons-material/AddOutlined"
import ComputerOutlinedIcon from "@mui/icons-material/ComputerOutlined"
import EditOutlinedIcon from "@mui/icons-material/EditOutlined"
import EmojiPeopleOutlinedIcon from "@mui/icons-material/EmojiPeopleOutlined"
import ExpandLessOutlinedIcon from "@mui/icons-material/ExpandLessOutlined"
import ExpandMoreOutlinedIcon from "@mui/icons-material/ExpandMoreOutlined"
import FileDownloadIcon from "@mui/icons-material/FileDownload"
import FileUploadIcon from "@mui/icons-material/FileUpload"
import FormatListNumberedOutlinedIcon from "@mui/icons-material/FormatListNumberedOutlined"
import KeyboardCommandKeyOutlinedIcon from "@mui/icons-material/KeyboardCommandKeyOutlined"
import KeyboardOutlinedIcon from "@mui/icons-material/KeyboardOutlined"

import {
  Button,
  Card,
  CardContent,
  CardHeader,
  Chip,
  IconButton,
  List,
  ListItem,
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
import { keyboadKeyCodes } from "@src/modules/key-bindings/data"
import { consumerControlCodes } from "@src/modules/key-bindings/data/consumerControlCodes"
import { systemControlCodes } from "@src/modules/key-bindings/data/systemControlCodes"
import { useStoredMacrosQuery } from "@src/modules/key-bindings/hooks"
import {
  type ConsumerControlMacroDefinition,
  type HIDAction,
  type HIDKeySequenceMacroDefinition,
  type MacroDefinition,
  MacroDefinitionType,
  type Shortcut,
  type ShortcutMacroDefinition,
  type ShortcutSequenceMacroDefinition,
  type SystemControlMacroDefinition
} from "@src/modules/key-bindings/models"
import { EditMacroDialog } from "@src/modules/key-bindings/ui/components/EditMacrosCard/EditMacroDialog"
import { type ComponentProps, useCallback, useState } from "react"

import classes from "./EditMacrosCard.module.css"

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
  switch (macro.type) {
    case MacroDefinitionType.Shortcut:
      return <ShortcutMacroCellContent macro={macro} />
    case MacroDefinitionType.ShortcutSequence:
      return <ShortcutSequenceMacroCellContent macro={macro} />
    case MacroDefinitionType.HIDKeySequence:
      return <HIDKeySequenceMacroCellContent macro={macro} />
    case MacroDefinitionType.ConsumerControl:
      return <ConsumerControlMacroCellContent macro={macro} />
    case MacroDefinitionType.SystemControl:
      return <SystemControlMacroCellContent macro={macro} />
    default:
      return null
  }
}

function isSequenceMacro(macro: MacroDefinition) {
  switch (macro.type) {
    case "ShortcutSequence":
    case "HIDKeySequence":
      return true
    default:
      return false
  }
}

const ShortcutMacroCellContent = (props: {
  macro: ShortcutMacroDefinition
}) => {
  const { macro } = props
  return (
    <ShortcutVisualization shortcut={macro.shortcut} />
  )
}

const ShortcutSequenceMacroCellContent = (props: {
  macro: ShortcutSequenceMacroDefinition
}) => {
  const { macro } = props

  const [isExpanded, setIsExpanded] = useState<boolean>(false)
  const handleExpansionToggled = useCallback(() => {
    setIsExpanded(s => !s)
  }, [setIsExpanded])

  return (
    <Stack direction={"row"} alignItems={"flex-start"} gap={1}>
      <List className={classes.ShortCutSequenceList} data-isexpanded={isExpanded}>
        {macro.shortcuts.map((shortcut, i) => {
          const key = `${shortcut.modifiers.join("-")}-${shortcut.hidCode}-${i}`
          return (
            <ListItem key={key}>
              {i + 1}.&nbsp;
              <ShortcutVisualization shortcut={shortcut} />
            </ListItem>
          )
        })}
      </List>
      <IconButton
        onClick={handleExpansionToggled}
        size={"small"}
      >
        {isExpanded
          ? <Tooltip title={"Show less"}><ExpandLessOutlinedIcon /></Tooltip>
          : <Tooltip title={"Show more"}><ExpandMoreOutlinedIcon /></Tooltip>
        }
      </IconButton>
    </Stack>
  )
}

const HIDKeySequenceMacroCellContent = (props: {
  macro: HIDKeySequenceMacroDefinition
}) => {
  const { macro } = props

  const [isExpanded, setIsExpanded] = useState<boolean>(false)
  const handleExpansionToggled = useCallback(() => {
    setIsExpanded(s => !s)
  }, [setIsExpanded])

  return (
    <Stack direction={"row"} alignItems={"flex-start"} gap={1}>
      <List className={classes.ShortCutSequenceList} data-isexpanded={isExpanded}>
        {macro.sequence.map((action, i) => {
          const key = `${action.type}-${action.hidCode}-${i}`
          return (
            <ListItem key={key}>
              {i + 1}.&nbsp;
              <HIDActionVisualization action={action} />
            </ListItem>
          )
        })}
      </List>
      <IconButton
        onClick={handleExpansionToggled}
        size={"small"}
      >
        {isExpanded
          ? <Tooltip title={"Show less"}><ExpandLessOutlinedIcon /></Tooltip>
          : <Tooltip title={"Show more"}><ExpandMoreOutlinedIcon /></Tooltip>
        }
      </IconButton>
    </Stack>
  )
}

const ShortcutVisualization = (props: {
  shortcut: Shortcut
}) => {
  const { shortcut } = props

  const modifiersLabel = shortcut.modifiers
    .map(hidCode => keyboadKeyCodes.byHidCode[hidCode])
    .map(it => it.hidDescription)
    .join(" + ")

  const hidKeyCode = keyboadKeyCodes.byHidCode[shortcut.hidCode]

  const keyCodeLabel = (
    <>
      <strong>{hidKeyCode.hidDescription}</strong>&nbsp;
      (0x{hidKeyCode.hidCode.toString(16).padStart(2, "0")})
    </>
  )

  return (
    <Stack direction={"row"} alignItems={"center"}>
      {shortcut.modifiers.length > 0 && (
        <>
          <Chip label={modifiersLabel} size={"small"} color={"secondary"} />
          <AddOutlinedIcon fontSize={"small"} />
        </>
      )}
      <Chip label={keyCodeLabel} size={"small"} color={"primary"} />
    </Stack>
  )
}


const HIDActionVisualization = (props: {
  action: HIDAction
}) => {
  const { action } = props

  const hidKeyCode = keyboadKeyCodes.byHidCode[action.hidCode]

  const keyCodeLabel = (
    <>
      <strong>{hidKeyCode.hidDescription}</strong>&nbsp;
      (0x{hidKeyCode.hidCode.toString(16).padStart(2, "0")})
    </>
  )

  const icon = action.type === "Press"
    ? <FileDownloadIcon fontSize={"small"} />
    : <FileUploadIcon fontSize={"small"} />

  const color: ComponentProps<typeof Chip>["color"] = action.type === "Press"
    ? "primary"
    : "warning"

  return (
    <Stack direction={"row"} alignItems={"center"}>
      <Tooltip title={action.type}>
        <Chip
          label={keyCodeLabel}
          size={"small"}
          color={color}
          icon={icon}
        />
      </Tooltip>
    </Stack>
  )
}

const ConsumerControlMacroCellContent = (props: {
  macro: ConsumerControlMacroDefinition
}) => {
  const { macro } = props
  const code = consumerControlCodes.byUsageId[macro.usageId]
  if (!code) {
    return null
  }
  return (
    <Chip
      label={<><strong>{code.usageName}</strong> (0x{macro.usageId.toString(16)})</>}
      size={"small"}
      color={"primary"}
    />
  )
}

const SystemControlMacroCellContent = (props: {
  macro: SystemControlMacroDefinition
}) => {
  const { macro } = props
  const code = systemControlCodes.byCode[macro.code]
  if (!code) {
    return null
  }
  return (
    <Chip
      label={<><strong>{code.description}</strong></>}
      size={"small"}
      color={"primary"}
    />
  )
}

const MacroTypeVisualization = (props: {
  type: MacroDefinitionType
}) => {
  switch (props.type) {
    case "Shortcut":
      return (
        <Tooltip title={"Shortcut"}>
          <Stack direction={"row"}>
            <KeyboardCommandKeyOutlinedIcon fontSize={"small"} />
          </Stack>
        </Tooltip>
      )
    case "ShortcutSequence":
      return (
        <Tooltip title={"Shortcut Sequence"}>
          <Stack direction={"row"}>
            <KeyboardCommandKeyOutlinedIcon fontSize={"small"} />
            <FormatListNumberedOutlinedIcon fontSize={"small"} />
          </Stack>
        </Tooltip>
      )
    case "HIDKeySequence":
      return (
        <Tooltip title={"HID Sequence"}>
          <Stack direction={"row"}>
            <KeyboardOutlinedIcon fontSize={"small"} />
            <FormatListNumberedOutlinedIcon fontSize={"small"} />
          </Stack>
        </Tooltip>
      )
    case MacroDefinitionType.ConsumerControl:
      return (
        <Tooltip title={"Consumer Control"}>
          <EmojiPeopleOutlinedIcon fontSize={"small"} />
        </Tooltip>
      )
    case MacroDefinitionType.SystemControl:
      return (
        <Tooltip title={"System Control"}>
          <ComputerOutlinedIcon fontSize={"small"} />
        </Tooltip>
      )
  }
}
