#include "ReadMetricsCommandHandler.h"

#include <string>
#include <esp_heap_caps.h>
#include <esp_system.h>
#include <LittleFS.h>

#include "utils/allocations/ArenaUtils.h"


ReadMetricsCommandHandler::ReadMetricsCommandHandler(const utils::metrics::MetricRegistry& metricRegistry)
    : CommandHandler("read.metrics"), metricRegistry(metricRegistry) {
}

ReadMetricsCommandHandler::~ReadMetricsCommandHandler() = default;


utils::void_result ReadMetricsCommandHandler::execute(
    const std::span<const std::string_view>& args,
    utils::commands::CommandResponseWriter& responseWriter,
    utils::allocations::Arena& arena
) {
    metricRegistry.forEach([&](const utils::metrics::MetricReport& report) {
        responseWriter.writeLineF("%.*s:%llu", report.name.length(), report.name.data(), report.value);
    });

    return utils::void_result::success();
}
