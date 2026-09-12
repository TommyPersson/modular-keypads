#include "ListRegisters.h"

#include <string>
#include <tfw/utils/allocations.h>

using namespace mkp::firmwares::master::commands;

ListRegisters::ListRegisters(
    const std::optional<tfw::utils::registers::RegisterManager*>& registers
) : CommandHandler("list.registers"), registers(registers) {
}

ListRegisters::~ListRegisters() = default;


tfw::utils::void_result ListRegisters::execute(
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
