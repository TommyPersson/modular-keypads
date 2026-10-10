import {
  type ConfigurationImportFailure,
  type ConfigurationImportProgress,
  ConfigurationImportStatus
} from "@src/modules/config-transfer/hooks"
import type { Meta, StoryObj } from "@storybook/react-vite"
import { useEffect, useState } from "react"
import { fn } from "storybook/test"
import { ImportConfigurationDialog } from "./ImportConfigurationDialog"

const failures: ConfigurationImportFailure[] = [
  { step: "Saving macro 'Paste Special'", error: new Error("command timeout") },
  { step: "Saving key binding 7 of 24", error: new Error("unknown.key.binding.type") },
  { step: "Saving setting 'typing.delay'", error: new Error("unknown.setting") },
]

const progress = (overrides: Partial<ConfigurationImportProgress>): ConfigurationImportProgress => ({
  status: ConfigurationImportStatus.Running,
  completed: 0,
  total: 42,
  currentStep: "",
  failures: [],
  ...overrides,
})

const meta = {
  title: "Config Transfer/Import Configuration Dialog",
  component: ImportConfigurationDialog,
  args: {
    onCancel: fn(),
    onClose: fn(),
  },
} satisfies Meta<typeof ImportConfigurationDialog>

export default meta

type Story = StoryObj<typeof meta>

export const Running: Story = {
  args: {
    progress: progress({ completed: 12, currentStep: "Saving macro 'Copy'" }),
  },
}

export const RunningWithFailures: Story = {
  args: {
    progress: progress({ completed: 30, currentStep: "Saving key binding 13 of 24", failures: failures.slice(0, 2) }),
  },
}

export const Cancelling: Story = {
  args: {
    progress: progress({
      status: ConfigurationImportStatus.Cancelling,
      completed: 20,
      currentStep: "Saving key binding 3 of 24",
    }),
  },
}

export const Completed: Story = {
  args: {
    progress: progress({ status: ConfigurationImportStatus.Completed, completed: 42 }),
  },
}

export const CompletedWithFailures: Story = {
  args: {
    progress: progress({ status: ConfigurationImportStatus.Completed, completed: 42, failures }),
  },
}

export const ManyFailures: Story = {
  args: {
    progress: progress({
      status: ConfigurationImportStatus.Completed,
      completed: 42,
      failures: Array.from({ length: 20 }, (_, index) => ({
        step: `Saving key binding ${index + 1} of 24`,
        error: new Error("command timeout"),
      })),
    }),
  },
}

export const Cancelled: Story = {
  args: {
    progress: progress({ status: ConfigurationImportStatus.Cancelled, completed: 20, failures: failures.slice(0, 1) }),
  },
}

// Steps through a fake import, with every fifth item failing, to see the dialog change over time
const SimulatedImportDialog = (props: { total: number }) => {
  const { total } = props

  const [simulatedProgress, setSimulatedProgress] = useState<ConfigurationImportProgress>(progress({ total }))

  useEffect(() => {
    const timer = setInterval(() => {
      setSimulatedProgress(previous => {
        if (previous.status !== ConfigurationImportStatus.Running) {
          return previous
        }

        const completed = previous.completed + 1
        const failure = { step: `Saving item ${completed} of ${previous.total}`, error: new Error("command timeout") }
        return {
          ...previous,
          completed,
          currentStep: `Saving item ${completed + 1} of ${previous.total}`,
          status: completed < previous.total ? ConfigurationImportStatus.Running : ConfigurationImportStatus.Completed,
          failures: completed % 5 === 0 ? [...previous.failures, failure] : previous.failures,
        }
      })
    }, 150)

    return () => clearInterval(timer)
  }, [])

  return (
    <ImportConfigurationDialog
      progress={simulatedProgress}
      onCancel={() => setSimulatedProgress(previous => ({ ...previous, status: ConfigurationImportStatus.Cancelled }))}
      onClose={() => setSimulatedProgress(progress({ total }))}
    />
  )
}

export const Simulated: Story = {
  args: {
    progress: progress({}),
  },
  render: (args) => <SimulatedImportDialog total={args.progress.total} />,
}
