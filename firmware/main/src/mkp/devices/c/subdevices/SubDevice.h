#pragma once
#include <SPI.h>

#include "mkp/devices/common/Device.h"
#include "tfw/hal/gpio/InputPin.h"
#include "tfw/hal/gpio/OutputPin.h"

namespace mkp::devices::c {
    class SubDevice : public tfw::utils::observables::Observer<tfw::hal::gpio::InputPinInterruptEvent>  {
    public:
        explicit SubDevice(
            uint8_t number,
            SPIClass& spi,
            tfw::hal::gpio::OutputPin& pinCS,
            tfw::hal::gpio::InputPin& pinINT
        );
        ~SubDevice() override;

        void setup();
        void update();

        void observe(const tfw::hal::gpio::InputPinInterruptEvent& event) override;

        tfw::utils::observables::Observable<common::DeviceSwitchEvent>& onSwitchEvent() { return deviceSwitchEventSubject; }

    private:
        static constexpr uint32_t DEFAULT_PAUSE_US = 50;

        uint8_t number;
        SPIClass& spi;
        tfw::hal::gpio::OutputPin& pinCS;
        tfw::hal::gpio::InputPin& pinINT;
        volatile bool eventsAvailable;

        tfw::utils::observables::Subject<common::DeviceSwitchEvent> deviceSwitchEventSubject;

        bool transact(uint8_t command, uint8_t* response, size_t length, uint32_t pauseUs = DEFAULT_PAUSE_US);
    };
}
