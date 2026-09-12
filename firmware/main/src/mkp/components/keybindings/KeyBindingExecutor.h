#pragma once

#include <tfw/hal/metrics.h>
#include <tfw/hal/usb.h>

#include "mkp/components/macros/MacroStorage.h"
#include "mkp/devices/common/Device.h"

#include "KeyBindingStorage.h"
#include "TestModeController.h"

namespace mkp::components::keybindings {
    struct CompiledMacro {
        uint16_t macroId;
        std::vector<std::shared_ptr<tfw::hal::usb::Action>> actions;
    };

    class KeyBindingExecutor
        : tfw::utils::observables::Observer<macros::MacroSaved>,
          tfw::utils::observables::Observer<macros::MacroRemoved>,
          tfw::utils::observables::Observer<KeyBindingSet>,
          tfw::utils::observables::Observer<KeyBindingCleared>,
          tfw::utils::observables::Observer<devices::common::DeviceSwitchEvent>,
          tfw::utils::observables::Observer<devices::common::DeviceRotaryEncoderEvent> {
    public:
        KeyBindingExecutor(
            macros::MacroStorage& macroStorage,
            KeyBindingStorage& keyBindingStorage,
            TestModeController& testModeController,
            tfw::hal::usb::Connection& usbConnection,
            tfw::hal::metrics::MetricRegistry& metricRegistry
        );
        ~KeyBindingExecutor() override;

        void setup();
        void loop();

        uint32_t getNumAssigned() const { return keyBindings.size(); }

        void observe(const macros::MacroSaved& event) override;
        void observe(const macros::MacroRemoved& event) override;
        void observe(const KeyBindingSet& event) override;
        void observe(const KeyBindingCleared& event) override;
        void observe(const devices::common::DeviceSwitchEvent& event) override;
        void observe(const devices::common::DeviceRotaryEncoderEvent& event) override;

    private:
        void refreshCompiledMacros();
        void refreshKeyBindings();

        std::shared_ptr<KeyBinding> findKeyBinding(const devices::common::DeviceSwitchEvent& event);
        std::shared_ptr<KeyBinding> findKeyBinding(const devices::common::DeviceRotaryEncoderEvent& event);

        void executeMacroFor(const std::shared_ptr<KeyBinding>& keyBinding);

        bool macrosNeedRefresh = true;
        bool keyBindingsNeedRefresh = true;

        macros::MacroStorage& macroStorage;
        KeyBindingStorage& keyBindingStorage;
        TestModeController& testModeController;
        tfw::hal::usb::Connection& usbConnection;

        std::vector<std::shared_ptr<KeyBinding>> keyBindings;
        std::vector<std::shared_ptr<CompiledMacro>> macros;
    };
}
