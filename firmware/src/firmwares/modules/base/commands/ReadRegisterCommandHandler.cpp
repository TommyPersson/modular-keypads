#include "ReadRegisterCommandHandler.h"

#include <string>
#include <utils/allocations/ArenaUtils.h>


ReadRegisterCommandHandler::ReadRegisterCommandHandler(const RegisterManager& registers) :
    CommandHandler("read.register"), registers(registers) {
}

ReadRegisterCommandHandler::~ReadRegisterCommandHandler() = default;


void ReadRegisterCommandHandler::execute(
    const std::span<const std::string_view>& args,
    CommandResponseWriter& responseWriter,
    Arena& arena
    ) {
    const auto registerName = std::string(args[0]);
    const auto value = registers.read(registerName);

    responseWriter.writeLine(arena::strings::sprintf(arena, "0x%02x", value));
}
