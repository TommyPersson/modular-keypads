#include "EspMetrics.h"

#include <esp_heap_caps.h>
#include <esp_system.h>
#include <multi_heap.h>

namespace firmware::metrics::esp {
    void register_all(utils::metrics::MetricRegistry& registry) {
        registry.add(utils::metrics::lambda_gauge("esp.memory.global.free_heap_size", []() {
            return esp_get_free_heap_size();
        }));

        registry.add(utils::metrics::lambda_gauge("esp.memory.global.free_internal_heap_size", []() {
            return esp_get_free_internal_heap_size();
        }));

        registry.add(utils::metrics::lambda_gauge("esp.memory.global.minimum_free_heap_size", []() {
            return esp_get_minimum_free_heap_size();
        }));

        registry.add(utils::metrics::lambda_gauge("esp.memory.malloc_cap.total_free_bytes", []() {
            multi_heap_info_t info;
            heap_caps_get_info(&info, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
            return info.total_free_bytes;
        }));

        registry.add(utils::metrics::lambda_gauge("esp.memory.malloc_cap.minimum_free_bytes", []() {
            multi_heap_info_t info;
            heap_caps_get_info(&info, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
            return info.minimum_free_bytes;
        }));

        registry.add(utils::metrics::lambda_gauge("esp.memory.malloc_cap.largest_free_block", []() {
            multi_heap_info_t info;
            heap_caps_get_info(&info, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
            return info.largest_free_block;
        }));
    }
}
