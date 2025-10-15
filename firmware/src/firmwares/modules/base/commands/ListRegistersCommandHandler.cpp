#include "ListRegistersCommandHandler.h"

#include <string>
#include <utils/allocations/ArenaUtils.h>


ListRegistersCommandHandler::ListRegistersCommandHandler(const RegisterManager& registers, Logger& logger) :
    CommandHandler("list.registers", logger), registers(registers) {
}

ListRegistersCommandHandler::~ListRegistersCommandHandler() = default;


std::string ListRegistersCommandHandler::execute(const std::span<const std::string_view>& args, Arena& arena) {
    const auto& names = registers.listNames();
    return arena::strings::join(names, ",", arena);
}
