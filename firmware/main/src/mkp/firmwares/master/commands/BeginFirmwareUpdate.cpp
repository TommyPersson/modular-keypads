#include "BeginFirmwareUpdate.h"

#include <tfw/utils/strings.h>

using namespace mkp::firmwares::master::commands;

BeginFirmwareUpdate::BeginFirmwareUpdate(
    mkp::components::firmware::FirmwareUpdater& firmwareUpdater
) : CommandHandler("begin.firmware.update"),
    firmwareUpdater(firmwareUpdater) {
}

BeginFirmwareUpdate::~BeginFirmwareUpdate() = default;

tfw::utils::void_result BeginFirmwareUpdate::execute(
    const std::span<const std::string_view>& args,
    tfw::utils::commands::CommandResponseWriter& responseWriter,
    tfw::utils::allocations::Arena& arena
) {
    if (args.size() < 1) {
        return tfw::utils::void_result::error("missing.firmware.size");
    }

    return firmwareUpdater.begin(tfw::utils::strings::atou32(args[0]));
}
