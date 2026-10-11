import FileDownloadOutlinedIcon from "@mui/icons-material/FileDownloadOutlined"
import FileUploadOutlinedIcon from "@mui/icons-material/FileUploadOutlined"
import { Button, ButtonGroup, Popover } from "@mui/material"
import { ErrorAlert } from "@src/modules/common/components"
import { useConfigurationImport } from "@src/modules/config-transfer/hooks"
import { createConfigurationExport, parseConfigurationExport } from "@src/modules/config-transfer/models"
import { ImportConfigurationDialog } from "@src/modules/config-transfer/ui/components/ImportConfigurationDialog"
import { useKeyBindingsQuery } from "@src/modules/key-bindings/hooks"
import { useStoredMacrosQuery } from "@src/modules/macros/hooks"
import { useSettingsQuery } from "@src/modules/settings/hooks"
import { useConfirm } from "@src/utils/hooks/useConfirm"
import { DateTime } from "luxon"
import * as React from "react"
import { useCallback, useRef, useState } from "react"

export const ConfigurationTransferButtons = () => {
  const macrosQuery = useStoredMacrosQuery()
  const keyBindingsQuery = useKeyBindingsQuery()
  const settingsQuery = useSettingsQuery()

  const confirm = useConfirm()
  const configurationImport = useConfigurationImport()

  const fileInputRef = useRef<HTMLInputElement | null>(null)
  const buttonGroupRef = useRef<HTMLDivElement | null>(null)
  const [fileError, setFileError] = useState<Error | null>(null)

  const handleExportClick = useCallback(() => {
    const configuration = createConfigurationExport(
      macrosQuery.data ?? [],
      keyBindingsQuery.data ?? [],
      settingsQuery.data ?? [],
    )
    const blob = new Blob([JSON.stringify(configuration, null, 2)], { type: "application/json" })
    const url = URL.createObjectURL(blob)

    const link = document.createElement("a")
    link.href = url
    link.download = `keypad-configuration-${DateTime.now().toFormat("yyyy-MM-dd-HHmmss")}.json`
    link.click()

    URL.revokeObjectURL(url)
  }, [macrosQuery.data, keyBindingsQuery.data, settingsQuery.data])

  const handleImportClick = useCallback(() => {
    fileInputRef.current?.click()
  }, [])

  const handleFileSelected = useCallback(async (event: React.ChangeEvent<HTMLInputElement>) => {
    const file = event.target.files?.[0]
    // Clear the selection so that the same file can be picked again
    event.target.value = ""
    if (!file) {
      return
    }

    let configuration
    try {
      configuration = parseConfigurationExport(await file.text())
    } catch (error) {
      setFileError(error as Error)
      return
    }

    const { confirmed } = await confirm({
      title: "Import configuration",
      content: (
        <>
          Import <strong>{configuration.macros.length}</strong> macros,
          {" "}<strong>{configuration.keyBindings.length}</strong> key bindings
          and <strong>{configuration.settings.length}</strong> settings from <em>{file.name}</em>?
          Existing macros with the same id, key bindings for the same triggers and the imported settings will be
          overwritten.
        </>
      ),
    })

    if (confirmed) {
      await configurationImport.start(configuration)
    }
  }, [confirm, configurationImport])

  const isLoaded = macrosQuery.isSuccess && keyBindingsQuery.isSuccess && settingsQuery.isSuccess

  return (
    <>
      <ButtonGroup ref={buttonGroupRef} variant={"text"}>
        <Button startIcon={<FileUploadOutlinedIcon />} onClick={handleImportClick}>
          Import
        </Button>
        <Button
          startIcon={<FileDownloadOutlinedIcon />}
          onClick={handleExportClick}
          disabled={!isLoaded}
          loading={macrosQuery.isLoading || keyBindingsQuery.isLoading || settingsQuery.isLoading}
        >
          Export
        </Button>
      </ButtonGroup>
      <input
        ref={fileInputRef}
        type={"file"}
        accept={".json,application/json"}
        hidden
        onChange={handleFileSelected}
      />
      <Popover
        open={!!fileError}
        anchorEl={buttonGroupRef.current}
        anchorOrigin={{ vertical: "top", horizontal: "left" }}
        transformOrigin={{ vertical: "bottom", horizontal: "left" }}
        onClose={() => setFileError(null)}
      >
        <ErrorAlert title={"Unable to read configuration file"} error={fileError} onClose={() => setFileError(null)} />
      </Popover>
      <ImportConfigurationDialog
        progress={configurationImport.progress}
        onCancel={configurationImport.cancel}
        onClose={configurationImport.reset}
      />
    </>
  )
}
