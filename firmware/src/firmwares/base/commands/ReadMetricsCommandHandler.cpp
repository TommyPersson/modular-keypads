#include "ReadMetricsCommandHandler.h"

#include <string>
#include <esp_heap_caps.h>
#include <esp_system.h>
#include <LittleFS.h>

#include "utils/allocations/ArenaUtils.h"


ReadMetricsCommandHandler::ReadMetricsCommandHandler()
    : CommandHandler("read.metrics") {
}

ReadMetricsCommandHandler::~ReadMetricsCommandHandler() = default;


utils::void_result ReadMetricsCommandHandler::execute(
    const std::span<const std::string_view>& args,
    CommandResponseWriter& responseWriter,
    Arena& arena
) {
    responseWriter.writeLineF("esp.memory.global.free_heap_size:%lu", esp_get_free_heap_size());
    responseWriter.writeLineF("esp.memory.global.free_internal_heap_size:%lu", esp_get_free_internal_heap_size());
    responseWriter.writeLineF("esp.memory.global.minimum_free_heap_size:%lu", esp_get_minimum_free_heap_size());

    multi_heap_info_t info;
    heap_caps_get_info(&info, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    responseWriter.writeLineF("esp.memory.malloc_cap.total_free_bytes:%lu", static_cast<unsigned long>(info.total_free_bytes));
    responseWriter.writeLineF("esp.memory.malloc_cap.minimum_free_bytes:%lu", static_cast<unsigned long>(info.minimum_free_bytes));
    responseWriter.writeLineF("esp.memory.malloc_cap.largest_free_block:%lu",  static_cast<unsigned long>(info.largest_free_block));

    size_t totalBytes = LittleFS.totalBytes();
    size_t usedBytes = LittleFS.usedBytes();
    auto freeBytes = totalBytes - usedBytes;
    responseWriter.writeLineF("littlefs.total_bytes:%u", totalBytes);
    responseWriter.writeLineF("littlefs.used_bytes:%u", usedBytes);
    responseWriter.writeLineF("littlefs.free_bytes:%u", freeBytes);

    return utils::void_result::success();
}
