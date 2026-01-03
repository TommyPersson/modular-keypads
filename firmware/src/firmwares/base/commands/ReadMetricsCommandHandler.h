#pragma once

#include <Registers/Registers.h>

#include "utils/commands/CommandHandler.h"
#include "utils/metrics/Metrics.h"

class ReadMetricsCommandHandler final : public utils::commands::CommandHandler {
public:
    explicit ReadMetricsCommandHandler(const utils::metrics::MetricRegistry& metricRegistry);
    ~ReadMetricsCommandHandler() override;

    utils::void_result execute(
        const std::span<const std::string_view>& args,
        utils::commands::CommandResponseWriter& responseWriter,
        utils::allocations::Arena& arena
    ) override;

private:
    const utils::metrics::MetricRegistry& metricRegistry;
};
