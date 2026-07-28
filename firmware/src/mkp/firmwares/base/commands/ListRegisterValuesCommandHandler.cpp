#include "ListRegisterValuesCommandHandler.h"

#include <string>

#include <tfw/utils/allocations.h>


ListRegisterValuesCommandHandler::ListRegisterValuesCommandHandler(const std::optional<tfw::utils::registers::RegisterManager*>& registers)
    : CommandHandler("list.register.values"),
      registers(registers) {}

ListRegisterValuesCommandHandler::~ListRegisterValuesCommandHandler() = default;


tfw::utils::void_result ListRegisterValuesCommandHandler::execute(
    const std::span<const std::string_view>& args,
    tfw::utils::commands::CommandResponseWriter& responseWriter,
    tfw::utils::allocations::Arena& arena
) {
    if (!registers.has_value()) {
        return tfw::utils::void_result::error("registers.not.available");
    }

    const auto& registerDescriptors = (*(this->registers))->list();
    for (auto& reg : registerDescriptors) {
        const auto value = (*registers)->read(reg);
        responseWriter.writeLineF("%s:%02x", reg.name.c_str(), value);
    }

    return tfw::utils::void_result::success();
}
