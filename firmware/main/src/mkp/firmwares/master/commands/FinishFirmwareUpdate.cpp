#include "FinishFirmwareUpdate.h"

using namespace mkp::firmwares::master::commands;

FinishFirmwareUpdate::FinishFirmwareUpdate(
    mkp::components::firmware::FirmwareUpdater& firmwareUpdater
) : CommandHandler("finish.firmware.update"),
    firmwareUpdater(firmwareUpdater) {
}

FinishFirmwareUpdate::~FinishFirmwareUpdate() = default;

tfw::utils::void_result FinishFirmwareUpdate::execute(
    const std::span<const std::string_view>& args,
    tfw::utils::commands::CommandResponseWriter& responseWriter,
    tfw::utils::allocations::Arena& arena
) {
    return firmwareUpdater.finish();
}
