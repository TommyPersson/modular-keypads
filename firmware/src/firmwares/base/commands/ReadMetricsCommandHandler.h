#pragma once

#include <tfw/utils/commands.h>
#include <tfw/hal/metrics.h>

class ReadMetricsCommandHandler final : public tfw::utils::commands::CommandHandler {
public:
    explicit ReadMetricsCommandHandler(const tfw::utils::metrics::MetricRegistry& metricRegistry);
    ~ReadMetricsCommandHandler() override;

    tfw::utils::void_result execute(
        const std::span<const std::string_view>& args,
        tfw::utils::commands::CommandResponseWriter& responseWriter,
        tfw::utils::allocations::Arena& arena
    ) override;

private:
    const tfw::utils::metrics::MetricRegistry& metricRegistry;
};
