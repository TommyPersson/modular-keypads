#include "ReadRegisterCommandHandler.h"

#include <string>
#include <utils/allocations/ArenaUtils.h>


ReadRegisterCommandHandler::ReadRegisterCommandHandler(const RegisterManager& registers, Logger& logger) :
    CommandHandler("read.register", logger), registers(registers) {
}

ReadRegisterCommandHandler::~ReadRegisterCommandHandler() = default;


std::string ReadRegisterCommandHandler::execute(const std::span<const std::string_view>& args, Arena& arena) {
    const auto registerName = std::string(args[0]);
    const auto& reg = registers.get(registerName);
    const auto value = reg->read();

    return arena::strings::sprintf(arena, "0x%02x", value);
}
