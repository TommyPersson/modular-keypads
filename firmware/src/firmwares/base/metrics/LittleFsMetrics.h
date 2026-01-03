#pragma once

#include "utils/metrics/Metrics.h"

namespace firmware::metrics::littlefs {
    void register_all(utils::metrics::MetricRegistry& registry);
}