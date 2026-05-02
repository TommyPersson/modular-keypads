#include "ListRegistersCommandHandler.h"

#include <string>
#include <tfw/utils/allocations.h>


ListRegistersCommandHandler::ListRegistersCommandHandler(
    const std::optional<tfw::utils::registers::RegisterManager*>& registers
) : CommandHandler("list.registers"), registers(registers) {
}

ListRegistersCommandHandler::~ListRegistersCommandHandler() = default;


tfw::utils::void_result ListRegistersCommandHandler::execute(
    const std::span<const std::string_view>& args,
    tfw::utils::commands::CommandResponseWriter& responseWriter,
    tfw::utils::allocations::Arena& arena
) {
    if (!registers.has_value()) {
        return tfw::utils::void_result::error("registers.not.available");
    }

    for (auto& descriptor : (*registers)->list()) {
        responseWriter.writeLine(descriptor.name);
    }

    return tfw::utils::void_result::success();
}
