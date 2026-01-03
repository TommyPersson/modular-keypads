#pragma once

#include "utils/metrics/Metrics.h"

namespace firmware::metrics::esp {
    void register_all(utils::metrics::MetricRegistry& registry);
}