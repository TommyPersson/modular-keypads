#include "ListRegisterValuesCommandHandler.h"

#include <string>

#include "utils/allocations/ArenaUtils.h"


ListRegisterValuesCommandHandler::ListRegisterValuesCommandHandler(const std::optional<RegisterManager*>& registers)
    : CommandHandler("list.register.values"),
      registers(registers) {}

ListRegisterValuesCommandHandler::~ListRegisterValuesCommandHandler() = default;


void ListRegisterValuesCommandHandler::execute(
    const std::span<const std::string_view>& args,
    CommandResponseWriter& responseWriter,
    Arena& arena
) {
    if (!registers.has_value()) {
        return;
    }

    const auto& registerDescriptors = (*(this->registers))->list();
    for (auto& reg : registerDescriptors) {
        const auto value = (*registers)->read(reg);
        responseWriter.writeLineF("%s:%02x", reg.name.c_str(), value);
    }
}
