#pragma once

#include <memory>

#include "utils/time/Time.h"

namespace utils::led::animations {
    struct Animation {
        Animation(uint32_t durationMs, uint64_t startTimeMs) : durationMs(durationMs), startTimeMs(startTimeMs) {
        }

        virtual ~Animation() = default;
        virtual uint32_t getColor(uint64_t time) = 0;

        bool isFinished(uint64_t time) const {
            return time > startTimeMs + durationMs;
        }

        uint32_t durationMs;
        uint64_t startTimeMs;
    };

    struct Pulse : Animation {
        Pulse(
            uint32_t color,
            uint32_t durationMs,
            uint64_t startTimeMs
        ) : Animation(durationMs, startTimeMs),
            color(color) {
        }

        ~Pulse() override = default;

        uint32_t getColor(uint64_t time) override {
            auto intervalMs = 500;
            auto tNow = (time - startTimeMs) % intervalMs;
            auto fraction = tNow / static_cast<float>(intervalMs);

            if (fraction > 0.5) {
                fraction = 1.0 - fraction; // ensures 0..0.5..0
            }

            fraction = fraction * 2; // back to 0..1

            auto w = color >> 24 & 0xFF;
            auto r = color >> 16 & 0xFF;
            auto g = color >> 8 & 0xFF;
            auto b = color >> 0 & 0xFF;

            auto nextColor = 0
                | (uint32_t)(fraction * w) << 24
                | (uint32_t)(fraction * r) << 16
                | (uint32_t)(fraction * g) << 8
                | (uint32_t)(fraction * b) << 0;

            return nextColor;
        }

        uint32_t color;
    };

    inline std::shared_ptr<Animation> pulse(uint32_t color, uint32_t durationMs) {
        return std::make_shared<Pulse>(color, durationMs, utils::time::millis());
    }
}
