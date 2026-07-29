#pragma once

#include <tfw/utils/commands.h>
#include <tfw/hal/metrics.h>

namespace mkp::firmwares::master::commands {
    class ReadMetrics final : public tfw::utils::commands::CommandHandler {
    public:
        explicit ReadMetrics(const tfw::hal::metrics::MetricRegistry& metricRegistry);
        ~ReadMetrics() override;

        tfw::utils::void_result execute(
            const std::span<const std::string_view>& args,
            tfw::utils::commands::CommandResponseWriter& responseWriter,
            tfw::utils::allocations::Arena& arena
        ) override;

    private:
        const tfw::hal::metrics::MetricRegistry& metricRegistry;
    };
}