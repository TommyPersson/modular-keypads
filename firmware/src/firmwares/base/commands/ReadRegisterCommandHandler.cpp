#include "ReadRegisterCommandHandler.h"

#include <string>
#include <tfw/utils/allocations.h>


ReadRegisterCommandHandler::ReadRegisterCommandHandler(
    const std::optional<tfw::utils::registers::RegisterManager*>& registers
) : CommandHandler("read.register"),
    registers(registers) {
}

ReadRegisterCommandHandler::~ReadRegisterCommandHandler() = default;


tfw::utils::void_result ReadRegisterCommandHandler::execute(
    const std::span<const std::string_view>& args,
    tfw::utils::commands::CommandResponseWriter& responseWriter,
    tfw::utils::allocations::Arena& arena
) {
    if (!registers.has_value()) {
        responseWriter.writeLine("0x00");
        return tfw::utils::void_result::success();
    }

    const auto registerName = std::string(args[0]);
    const auto value = (*registers)->read(registerName);

    responseWriter.writeLineF("0x%02x", value);

    return tfw::utils::void_result::success();
}
