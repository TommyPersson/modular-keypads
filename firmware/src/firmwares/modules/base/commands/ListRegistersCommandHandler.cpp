#include "ListRegistersCommandHandler.h"

#include <string>
#include <utils/allocations/ArenaUtils.h>


ListRegistersCommandHandler::ListRegistersCommandHandler(const RegisterManager& registers, Logger& logger) :
    CommandHandler("list.registers", logger), registers(registers) {
}

ListRegistersCommandHandler::~ListRegistersCommandHandler() = default;


void ListRegistersCommandHandler::execute(
    const std::span<const std::string_view>& args,
    CommandResponseWriter& responseWriter,
    Arena& arena
    ) {
    for (auto& name : registers.listNames()) {
        responseWriter.writeLine(name);
    }
}
