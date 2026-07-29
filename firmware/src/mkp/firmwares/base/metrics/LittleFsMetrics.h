#pragma once

#include <tfw/hal/metrics.h>

namespace mkp::firmwares::base::metrics::littlefs {
    void register_all(tfw::hal::metrics::MetricRegistry& registry);
}