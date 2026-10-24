#include "AbortFirmwareUpdate.h"

using namespace mkp::firmwares::master::commands;

AbortFirmwareUpdate::AbortFirmwareUpdate(
    mkp::components::firmware::FirmwareUpdater& firmwareUpdater
) : CommandHandler("abort.firmware.update"),
    firmwareUpdater(firmwareUpdater) {
}

AbortFirmwareUpdate::~AbortFirmwareUpdate() = default;

tfw::utils::void_result AbortFirmwareUpdate::execute(
    const std::span<const std::string_view>& args,
    tfw::utils::commands::CommandResponseWriter& responseWriter,
    tfw::utils::allocations::Arena& arena
) {
    firmwareUpdater.abort();

    return tfw::utils::void_result::success();
}
