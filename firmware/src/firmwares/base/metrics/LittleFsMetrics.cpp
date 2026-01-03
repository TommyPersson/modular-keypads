#include "LittleFsMetrics.h"

#include <LittleFS.h>

namespace firmware::metrics::littlefs {
    void register_all(utils::metrics::MetricRegistry& registry) {
        registry.add(utils::metrics::lambda_gauge("littlefs.total_bytes", []() {
            return LittleFS.totalBytes();
        }));

        registry.add(utils::metrics::lambda_gauge("littlefs.used_bytes", []() {
            return LittleFS.usedBytes();
        }));

        registry.add(utils::metrics::lambda_gauge("littlefs.free_bytes", []() {
            const auto totalBytes = LittleFS.totalBytes();
            const auto usedBytes = LittleFS.usedBytes();
            return totalBytes - usedBytes;
        }));
    }
}
