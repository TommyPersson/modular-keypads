import RestartAltOutlinedIcon from "@mui/icons-material/RestartAltOutlined"
import {
  Button,
  Card,
  CardActions,
  CardContent,
  CardHeader,
  IconButton,
  InputAdornment,
  Table,
  TableBody,
  TableCell,
  TableHead,
  TableRow,
  TextField,
  Tooltip,
  Typography
} from "@mui/material"
import { CommandButton, EmptyTableRow, ErrorAlert } from "@src/modules/common/components"
import { SaveSettingsCommand, type SettingChange } from "@src/modules/settings/commands"
import { useSettingsQuery } from "@src/modules/settings/hooks"
import { getSettingDescription, type Setting } from "@src/modules/settings/models"
import { useCallback, useMemo, useState } from "react"

// Edited values by setting key, kept as entered so that invalid input can be shown
type EditedValues = Record<string, string>

export const EditSettingsCard = () => {
  const settingsQuery = useSettingsQuery()
  const settings = useMemo(() => settingsQuery.data ?? [], [settingsQuery.data])

  const [editedValues, setEditedValues] = useState<EditedValues>({})

  const handleChange = useCallback((key: string, value: string) => {
    setEditedValues(previous => ({ ...previous, [key]: value }))
  }, [])

  const handleDiscard = useCallback(() => {
    setEditedValues({})
  }, [])

  const changes = useMemo(() => getChanges(settings, editedValues), [settings, editedValues])

  return (
    <Card>
      <CardHeader title={"Settings"} subheader={"Settings that apply to the whole keypad"} />
      <ErrorAlert title={"Unable to load settings"} error={settingsQuery.error} />
      <CardContent style={{ paddingLeft: 0, paddingRight: 0 }}>
        <Table size={"small"}>
          <TableHead>
            <TableRow>
              <TableCell>Setting</TableCell>
              <TableCell style={{ width: 220 }}>Value</TableCell>
              <TableCell style={{ width: 0 }}></TableCell>
            </TableRow>
          </TableHead>
          <TableBody>
            {settings.length === 0 && (
              <EmptyTableRow>
                {settingsQuery.isLoading ? "Loading settings..." : "No settings available"}
              </EmptyTableRow>
            )}
            {settings.map(setting => (
              <SettingRow
                key={setting.key}
                setting={setting}
                editedValue={editedValues[setting.key]}
                onChange={handleChange}
              />
            ))}
          </TableBody>
        </Table>
      </CardContent>
      <CardActions sx={{ justifyContent: "flex-end" }}>
        <Button onClick={handleDiscard} disabled={Object.keys(editedValues).length === 0}>
          Discard changes
        </Button>
        <CommandButton
          command={SaveSettingsCommand}
          args={changes !== null ? { changes } : null}
          onSuccess={handleDiscard}
          variant={"contained"}
        />
      </CardActions>
    </Card>
  )
}

const SettingRow = (props: {
  setting: Setting
  editedValue: string | undefined
  onChange: (key: string, value: string) => void
}) => {
  const { setting, editedValue, onChange } = props

  const description = getSettingDescription(setting.key)

  const value = editedValue ?? setting.value.toString()
  const validationError = validate(setting, value)
  const isModified = editedValue !== undefined && editedValue !== setting.value.toString()

  const helperText = validationError
    ?? (isModified ? `Unsaved, currently ${setting.value}` : `${setting.minValue} to ${setting.maxValue}`)

  return (
    <TableRow hover>
      <TableCell>
        <Typography variant={"body1"}><strong>{description.title}</strong></Typography>
        {description.description && (
          <Typography variant={"body2"} color={"text.secondary"}>{description.description}</Typography>
        )}
        <Typography variant={"caption"} color={"text.secondary"}><code>{setting.key}</code></Typography>
      </TableCell>
      <TableCell>
        <TextField
          size={"small"}
          variant={"filled"}
          type={"number"}
          value={value}
          onChange={e => onChange(setting.key, e.target.value)}
          error={validationError !== null}
          color={isModified ? "warning" : undefined}
          focused={isModified || undefined}
          helperText={helperText}
          slotProps={{
            htmlInput: { min: setting.minValue, max: setting.maxValue, step: 1 },
            input: description.unit
              ? { endAdornment: <InputAdornment position={"end"}>{description.unit}</InputAdornment> }
              : undefined,
          }}
          fullWidth
        />
      </TableCell>
      <TableCell>
        <Tooltip title={`Reset to default (${setting.defaultValue})`}>
          <span>
            <IconButton
              disabled={value === setting.defaultValue.toString()}
              onClick={() => onChange(setting.key, setting.defaultValue.toString())}
            >
              <RestartAltOutlinedIcon />
            </IconButton>
          </span>
        </Tooltip>
      </TableCell>
    </TableRow>
  )
}

function validate(setting: Setting, value: string): string | null {
  if (!/^\d+$/.test(value.trim())) {
    return "Must be a whole number"
  }

  const number = parseInt(value, 10)
  if (number < setting.minValue || number > setting.maxValue) {
    return `Must be between ${setting.minValue} and ${setting.maxValue}`
  }

  return null
}

// Returns null when any edited value is invalid, so that nothing is saved
function getChanges(settings: Setting[], editedValues: EditedValues): SettingChange[] | null {
  const changes: SettingChange[] = []

  for (const setting of settings) {
    const editedValue = editedValues[setting.key]
    if (editedValue === undefined) {
      continue
    }

    if (validate(setting, editedValue) !== null) {
      return null
    }

    const value = parseInt(editedValue, 10).toString()
    if (value !== setting.value.toString()) {
      changes.push({ key: setting.key, value })
    }
  }

  return changes
}
