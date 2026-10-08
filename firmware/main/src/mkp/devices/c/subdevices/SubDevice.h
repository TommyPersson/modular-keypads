#pragma once
#include <SPI.h>

#include "mkp/devices/common/Device.h"
#include "mkp/firmwares/base/ServiceLocator.h"
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

    class SubDevice final
        : public common::Device,
          public tfw::utils::observables::Observer<tfw::hal::gpio::InputPinInterruptEvent> {
    public:
        explicit SubDevice(
            tfw::hal::time::Clock& clock,
            uint8_t address,
            uint64_t parentDeviceId,
            uint8_t number,
            SPIClass& spi,
            tfw::hal::gpio::OutputPin& pinCS,
            tfw::hal::gpio::InputPin& pinINT
        );
        ~SubDevice() override;

        void setup() override;
        void loop() override;

        const common::DeviceConfiguration& getConfiguration() const override;
        const std::vector<std::shared_ptr<common::DeviceCapability>>& getCapabilities() const override;

        tfw::utils::void_result flashIdentificationLights(uint32_t durationMs) override;
        tfw::utils::void_result flashButtonIdentificationLight(uint8_t buttonNumber, uint32_t durationMs) override;
        tfw::utils::void_result rename(const std::string_view& deviceName) override;

        void scan();

        void observe(const tfw::hal::gpio::InputPinInterruptEvent& event) override;

        tfw::utils::observables::Observable<SubDeviceConnectedEvent>& onConnected() { return connectedSubject; }

        tfw::utils::observables::Observable<SubDeviceDisconnectedEvent>& onDisconnected() {
            return disconnectedSubject;
        }

    private:
        static constexpr uint32_t DEFAULT_PAUSE_US = 50;

        uint8_t address;
        uint64_t parentDeviceId;
        uint8_t number;
        uint64_t deviceId = 0;
        uint8_t deviceType = 0;
        SPIClass& spi;
        tfw::hal::gpio::OutputPin& pinCS;
        tfw::hal::gpio::InputPin& pinINT;
        volatile bool eventsAvailable = false;
        volatile bool scanScheduled = true;
        uint8_t numMissedScans = 0;
        common::DeviceConfiguration configuration;
        std::unique_ptr<tfw::hal::time::Timer> scanTimer;

        tfw::utils::observables::Subject<SubDeviceConnectedEvent> connectedSubject;
        tfw::utils::observables::Subject<SubDeviceDisconnectedEvent> disconnectedSubject;

        void pollEvents();
        bool transact(uint8_t command, uint8_t* response, size_t length, uint32_t pauseUs = DEFAULT_PAUSE_US);

    public:
    };
}
