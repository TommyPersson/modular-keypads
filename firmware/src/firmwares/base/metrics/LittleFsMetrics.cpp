#include "LittleFsMetrics.h"

#include <LittleFS.h>

namespace firmware::metrics::littlefs {
    void register_all(tfw::hal::metrics::MetricRegistry& registry) {
        registry.add(tfw::hal::metrics::lambda_gauge("littlefs.total_bytes", []() {
            return LittleFS.totalBytes();
        }));

        registry.add(tfw::hal::metrics::lambda_gauge("littlefs.used_bytes", []() {
            return LittleFS.usedBytes();
        }));

        registry.add(tfw::hal::metrics::lambda_gauge("littlefs.free_bytes", []() {
            const auto totalBytes = LittleFS.totalBytes();
            const auto usedBytes = LittleFS.usedBytes();
            return totalBytes - usedBytes;
        }));
    }
}
