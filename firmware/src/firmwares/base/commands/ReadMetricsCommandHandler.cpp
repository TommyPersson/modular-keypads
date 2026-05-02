#include "ReadMetricsCommandHandler.h"

#include <string>
#include <esp_heap_caps.h>
#include <esp_system.h>
#include <LittleFS.h>

#include <tfw/utils/allocations.h>


ReadMetricsCommandHandler::ReadMetricsCommandHandler(const tfw::utils::metrics::MetricRegistry& metricRegistry)
    : CommandHandler("read.metrics"), metricRegistry(metricRegistry) {
}

ReadMetricsCommandHandler::~ReadMetricsCommandHandler() = default;


tfw::utils::void_result ReadMetricsCommandHandler::execute(
    const std::span<const std::string_view>& args,
    tfw::utils::commands::CommandResponseWriter& responseWriter,
    tfw::utils::allocations::Arena& arena
) {
    metricRegistry.forEach([&](const tfw::utils::metrics::MetricReport& report) {
        responseWriter.writeLineF("%.*s:%llu", report.name.length(), report.name.data(), report.value);
    });

    return tfw::utils::void_result::success();
}
