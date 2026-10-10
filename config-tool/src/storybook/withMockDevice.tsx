import type { Decorator } from "@storybook/react-vite"
import type { MockDeviceOptions } from "./MockDeviceFacade"
import { MockDeviceProvider } from "./MockDeviceProvider"

/**
 * Wraps stories in a mock device, configured by the `mockDevice` parameter.
 */
export const withMockDevice: Decorator = (Story, context) => {
  const options: MockDeviceOptions = context.parameters.mockDevice ?? { devices: [] }

  // Keyed by story, so that switching stories creates a new device
  return (
    <MockDeviceProvider key={context.id} options={options}>
      <Story />
    </MockDeviceProvider>
  )
}
