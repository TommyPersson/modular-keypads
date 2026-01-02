#include "ListRegistersCommandHandler.h"

#include <string>
#include <utils/allocations/ArenaUtils.h>


ListRegistersCommandHandler::ListRegistersCommandHandler(
    const std::optional<utils::registers::RegisterManager*>& registers
) : CommandHandler("list.registers"), registers(registers) {
}

ListRegistersCommandHandler::~ListRegistersCommandHandler() = default;


utils::void_result ListRegistersCommandHandler::execute(
    const std::span<const std::string_view>& args,
    utils::commands::CommandResponseWriter& responseWriter,
    utils::allocations::Arena& arena
) {
    if (!registers.has_value()) {
        return utils::void_result::error("registers.not.available");
    }

    for (auto& descriptor : (*registers)->list()) {
        responseWriter.writeLine(descriptor.name);
    }

    return utils::void_result::success();
}
