#pragma once

#include <set>
#include <utility>

#include <tfw/hal/metrics.h>
#include <tfw/hal/settings.h>
#include <tfw/hal/time/Clock.h>
#include <tfw/hal/usb.h>

#include "mkp/components/macros/MacroStorage.h"
#include "mkp/devices/common/Device.h"

#include "KeyBindingStorage.h"
#include "SequenceRunner.h"
#include "TestModeController.h"

namespace mkp::components::keybindings {
    struct CompiledMacro {
        uint16_t macroId;
        std::vector<std::shared_ptr<tfw::hal::usb::Action>> actions;
        // The steps of a sequence, which runs other macros rather than actions of its own
        std::vector<macros::SequenceStep> steps;
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
            tfw::hal::settings::DeviceSettings& deviceSettings,
            tfw::hal::metrics::MetricRegistry& metricRegistry,
            const tfw::hal::time::Clock& clock
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

        std::shared_ptr<KeyBinding> findKeyBinding(
            uint64_t deviceId,
            uint8_t switchNumber,
            tfw::hal::buttons::ButtonState state
        );
        std::shared_ptr<KeyBinding> findKeyBinding(const devices::common::DeviceRotaryEncoderEvent& event);

        void executeMacroFor(const std::shared_ptr<KeyBinding>& keyBinding);
        void executeSequenceStep(uint16_t macroId);
        std::shared_ptr<CompiledMacro> findCompiledMacro(uint16_t macroId);

        bool macrosNeedRefresh = true;
        bool keyBindingsNeedRefresh = true;

        macros::MacroStorage& macroStorage;
        KeyBindingStorage& keyBindingStorage;
        TestModeController& testModeController;
        tfw::hal::usb::Connection& usbConnection;
        tfw::hal::settings::DeviceSettings& deviceSettings;

        std::vector<std::shared_ptr<KeyBinding>> keyBindings;
        std::vector<std::shared_ptr<CompiledMacro>> macros;

        SequenceRunner sequenceRunner;

        // Presses of buttons with a long press binding, as device id and switch number. Their press binding is
        // executed on release, unless the press turned into a long press.
        std::set<std::pair<uint64_t, uint8_t>> pendingPresses;
    };
}
