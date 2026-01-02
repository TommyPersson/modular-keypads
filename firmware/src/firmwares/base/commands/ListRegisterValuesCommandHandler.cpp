#include "ListRegisterValuesCommandHandler.h"

#include <string>

#include "utils/allocations/ArenaUtils.h"


ListRegisterValuesCommandHandler::ListRegisterValuesCommandHandler(const std::optional<utils::registers::RegisterManager*>& registers)
    : CommandHandler("list.register.values"),
      registers(registers) {}

ListRegisterValuesCommandHandler::~ListRegisterValuesCommandHandler() = default;


utils::void_result ListRegisterValuesCommandHandler::execute(
    const std::span<const std::string_view>& args,
    utils::commands::CommandResponseWriter& responseWriter,
    utils::allocations::Arena& arena
) {
    if (!registers.has_value()) {
        return utils::void_result::error("registers.not.available");
    }

    const auto& registerDescriptors = (*(this->registers))->list();
    for (auto& reg : registerDescriptors) {
        const auto value = (*registers)->read(reg);
        responseWriter.writeLineF("%s:%02x", reg.name.c_str(), value);
    }

    return utils::void_result::success();
}
