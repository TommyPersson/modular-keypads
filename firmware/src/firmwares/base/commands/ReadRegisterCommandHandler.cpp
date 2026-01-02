#include "ReadRegisterCommandHandler.h"

#include <string>
#include <utils/allocations/ArenaUtils.h>


ReadRegisterCommandHandler::ReadRegisterCommandHandler(
    const std::optional<utils::registers::RegisterManager*>& registers
) : CommandHandler("read.register"),
    registers(registers) {
}

ReadRegisterCommandHandler::~ReadRegisterCommandHandler() = default;


utils::void_result ReadRegisterCommandHandler::execute(
    const std::span<const std::string_view>& args,
    utils::commands::CommandResponseWriter& responseWriter,
    utils::allocations::Arena& arena
) {
    if (!registers.has_value()) {
        responseWriter.writeLine("0x00");
        return utils::void_result::success();
    }

    const auto registerName = std::string(args[0]);
    const auto value = (*registers)->read(registerName);

    responseWriter.writeLineF("0x%02x", value);

    return utils::void_result::success();
}
