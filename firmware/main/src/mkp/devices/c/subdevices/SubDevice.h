#pragma once
#include <SPI.h>

#include "mkp/devices/common/Device.h"
#include "tfw/hal/gpio/InputPin.h"
#include "tfw/hal/gpio/OutputPin.h"

namespace mkp::devices::c {
    struct SubDeviceConnectedEvent {
        const uint8_t subDeviceNumber;
        const uint64_t subDeviceId;
        const uint8_t subDeviceType;
    };

    struct SubDeviceDisconnectedEvent {
        const uint8_t subDeviceNumber;
        const uint64_t subDeviceId;
    };

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

        void scan();

        void observe(const tfw::hal::gpio::InputPinInterruptEvent& event) override;

        tfw::utils::observables::Observable<SubDeviceConnectedEvent>& onConnected() { return connectedSubject; }
        tfw::utils::observables::Observable<SubDeviceDisconnectedEvent>& onDisconnected() { return disconnectedSubject; }
        tfw::utils::observables::Observable<common::DeviceSwitchEvent>& onSwitchEvent() { return deviceSwitchEventSubject; }

    private:
        static constexpr uint32_t DEFAULT_PAUSE_US = 50;

        uint8_t number;
        uint64_t deviceId = 0;
        uint8_t deviceType = 0;
        SPIClass& spi;
        tfw::hal::gpio::OutputPin& pinCS;
        tfw::hal::gpio::InputPin& pinINT;
        volatile bool eventsAvailable = false;
        volatile bool scanScheduled = false;

        tfw::utils::observables::Subject<common::DeviceSwitchEvent> deviceSwitchEventSubject;
        tfw::utils::observables::Subject<SubDeviceConnectedEvent> connectedSubject;
        tfw::utils::observables::Subject<SubDeviceDisconnectedEvent> disconnectedSubject;

        void pollEvents();
        bool transact(uint8_t command, uint8_t* response, size_t length, uint32_t pauseUs = DEFAULT_PAUSE_US);
    };
}
