#include "ReadMetrics.h"

#include <string>

#include <tfw/utils/allocations.h>

using namespace mkp::firmwares::master::commands;

ReadMetrics::ReadMetrics(const tfw::hal::metrics::MetricRegistry& metricRegistry)
    : CommandHandler("read.metrics"), metricRegistry(metricRegistry) {
}

ReadMetrics::~ReadMetrics() = default;


tfw::utils::void_result ReadMetrics::execute(
    const std::span<const std::string_view>& args,
    tfw::utils::commands::CommandResponseWriter& responseWriter,
    tfw::utils::allocations::Arena& arena
) {
    metricRegistry.forEach([&](const tfw::hal::metrics::MetricReport& report) {
        responseWriter.writeLineF("%.*s:%llu", report.name.length(), report.name.data(), report.value);
    });

    return tfw::utils::void_result::success();
}
