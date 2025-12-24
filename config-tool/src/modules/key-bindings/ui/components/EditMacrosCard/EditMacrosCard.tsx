import AddOutlinedIcon from "@mui/icons-material/AddOutlined"
import ExpandLessOutlinedIcon from "@mui/icons-material/ExpandLessOutlined"
import ExpandMoreOutlinedIcon from "@mui/icons-material/ExpandMoreOutlined"
import FileDownloadIcon from "@mui/icons-material/FileDownload"
import FileUploadIcon from "@mui/icons-material/FileUpload"
import FormatListNumberedOutlinedIcon from "@mui/icons-material/FormatListNumberedOutlined"
import KeyboardCommandKeyOutlinedIcon from "@mui/icons-material/KeyboardCommandKeyOutlined"
import KeyboardOutlinedIcon from "@mui/icons-material/KeyboardOutlined"
import TimesOneMobiledataOutlinedIcon from "@mui/icons-material/TimesOneMobiledataOutlined"

import {
  Card,
  CardContent,
  CardHeader,
  Chip,
  Divider,
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
import { EmptyTableRow } from "@src/modules/common/components"
import { keyboadKeyCodes } from "@src/modules/key-bindings/data"
import {
  type HIDAction,
  type HIDKeySequenceMacroDefinition,
  type MacroDefinition,
  type MacroDefinitionType,
  ModifierKey,
  type Shortcut,
  type ShortcutMacroDefinition,
  type ShortcutSequenceMacroDefinition
} from "@src/modules/key-bindings/models"
import { type ComponentProps, useCallback, useState } from "react"

import classes from "./EditMacrosCard.module.css"

export const EditMacrosCard = () => {
  return (
    <Card>
      <CardHeader
        title={"Macros"}
        subheader={"Create macros to be able to bind them to keys"}
      />
      <Divider />
      <>
        <CardHeader title={"Create Macro"}>

        </CardHeader>
        <CardContent>

        </CardContent>
      </>
      <Divider />
      <ExistingMacrosCardContent macros={testMacros} />
    </Card>
  )
}

const ExistingMacrosCardContent = (props: {
  macros: MacroDefinition[]
}) => {

  const { macros } = props

  return (
    <>
      <CardHeader title={"Existing Macros"} />
      <CardContent style={{ paddingLeft: 0, paddingRight: 0 }}>
        <Table size={"small"}>
          <TableHead>
            <TableRow>
              <TableCell style={{ width: 0 }}>ID</TableCell>
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
              <MacroDefinitionRow key={macro.id} macro={macro} />
            )}
          </TableBody>
        </Table>
      </CardContent>
    </>
  )
}

const MacroDefinitionRow = (props: {
  macro: MacroDefinition
}) => {
  const { macro } = props

  const definitionCellContent = createMacroDefinitionCellContent(macro)

  const verticalAlign = isSequenceMacro(macro) ? "top" : undefined

  return (
    <TableRow hover>
      <TableCell style={{ whiteSpace: "nowrap", verticalAlign }}>{macro.id}</TableCell>
      <TableCell style={{ whiteSpace: "nowrap", verticalAlign }}>
        <MacroTypeVisualization type={macro.type} />
      </TableCell>
      <TableCell>{definitionCellContent}</TableCell>
      <TableCell style={{ verticalAlign }}>Edit yo</TableCell>
    </TableRow>
  )
}

function createMacroDefinitionCellContent(macro: MacroDefinition) {
  switch (macro.type) {
    case "Shortcut":
      return <ShortcutMacroCellContent macro={macro} />
    case "ShortcutSequence":
      return <ShortcutSequenceMacroCellContent macro={macro} />
    case "HIDKeySequence":
      return <HIDKeySequenceMacroCellContent macro={macro} />
    default:
      return null
  }
}

function isSequenceMacro(macro: MacroDefinition) {
  switch (macro.type) {
    case "ShortcutSequence":
    case "HIDKeySequence":
      return true;
    default:
      return false;
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
            <ListItem>
              {i + 1}.&nbsp;
              <ShortcutVisualization key={key} shortcut={shortcut} />
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
            <ListItem>
              {i + 1}.&nbsp;
              <HIDActionVisualization key={key} action={action} />
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

  const modifiersLabel = shortcut.modifiers.join(" + ")
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

const MacroTypeVisualization = (props: {
  type: MacroDefinitionType
}) => {
  switch (props.type) {
    case "Shortcut":
      return (
        <Tooltip title={"Single Shortcut"}>
          <Stack direction={"row"}>
            <KeyboardCommandKeyOutlinedIcon fontSize={"small"} />
            <TimesOneMobiledataOutlinedIcon fontSize={"small"} />
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
  }
}

const testMacros: MacroDefinition[] = [
  {
    id: "My First Macro",
    type: "Shortcut",
    shortcut: {
      modifiers: [ModifierKey.Control, ModifierKey.Shift],
      hidCode: 0x04 // A
    }
  },
  {
    id: "IntelliJ: Run",
    type: "Shortcut",
    shortcut: {
      modifiers: [ModifierKey.Shift],
      hidCode: 0x43 // F10
    }
  },
  {
    id: "IntelliJ: Debug",
    type: "Shortcut",
    shortcut: {
      modifiers: [ModifierKey.Shift],
      hidCode: 0x42 // F9
    }
  },
  {
    id: "IntelliJ: Stop",
    type: "Shortcut",
    shortcut: {
      modifiers: [ModifierKey.Control],
      hidCode: 0x3b // F2
    }
  },
  {
    id: "Vim: Save & Exit",
    type: "ShortcutSequence",
    shortcuts: [
      {
        modifiers: [],
        hidCode: 0x29 // Escape
      },
      {
        modifiers: [ModifierKey.Shift],
        hidCode: 0x33 // ; :
      },
      {
        modifiers: [],
        hidCode: 0x1a // W
      },
      {
        modifiers: [],
        hidCode: 0x14 // Q
      },
      {
        modifiers: [],
        hidCode: 0x28 // Enter
      }
    ]
  },
  {
    id: "Vim: Force Exit",
    type: "ShortcutSequence",
    shortcuts: [
      {
        modifiers: [],
        hidCode: 0x29 // Escape
      },
      {
        modifiers: [ModifierKey.Shift],
        hidCode: 0x33 // ; :
      },
      {
        modifiers: [],
        hidCode: 0x14 // Q
      },
      {
        modifiers: [ModifierKey.Shift],
        hidCode: 0x1e // 1 !
      },
      {
        modifiers: [],
        hidCode: 0x28 // Enter
      }
    ]
  },
  {
    id: "Raw HID Sequence",
    type: "HIDKeySequence",
    sequence: [
      {
        type: "Press",
        hidCode: 0x29
      },
      {
        type: "Press",
        hidCode: 0x30
      },
      {
        type: "Release",
        hidCode: 0x30
      },
      {
        type: "Release",
        hidCode: 0x29
      }
    ]
  }
]