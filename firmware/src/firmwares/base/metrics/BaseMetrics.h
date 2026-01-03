#pragma once

#include "utils/metrics/Metrics.h"

namespace firmware::metrics::base {
    void register_all(utils::metrics::MetricRegistry& registry);
}