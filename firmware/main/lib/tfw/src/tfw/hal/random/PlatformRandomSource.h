#pragma once

#include "RandomSource.h"

#ifdef ESP32
#include <bootloader_random.h>
#include <esp_random.h>
#else
#include <random>
#endif

namespace tfw::hal::random {
    /**
     * Random bytes from the hardware random number generator.
     *
     * On ESP32: Without Wi-Fi or Bluetooth running, the generator is only truly random while the SAR ADC entropy
     * source is enabled, so it's enabled while filling. Enabling and disabling it takes a few microseconds. The ADC
     * can't be used meanwhile, so this must not be called while it's in use, such as from another task.
     * On other platforms: Uses std::random_device
     */
    class PlatformRandomSource : public RandomSource {
    public:
        void fill(uint8_t* buffer, const size_t length) override {
#ifdef ESP32
            bootloader_random_enable();
            esp_fill_random(buffer, length);
            bootloader_random_disable();
#else
            std::random_device device;
            for (size_t i = 0; i < length; i++) {
                buffer[i] = static_cast<uint8_t>(device());
            }
#endif
        }
    };
}
