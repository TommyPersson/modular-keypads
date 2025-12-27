#include "DeleteMacroCommandHandler.h"

#include <utils/strings.h>

using namespace common::macros;

namespace {
    auto logger = common::logging::createLogger("DeleteMacroCommandHandler");
}

DeleteMacroCommandHandler::DeleteMacroCommandHandler(MacroStorage& macroStorage)
    : CommandHandler("delete.macro"),
      macroStorage(macroStorage) {
}

DeleteMacroCommandHandler::~DeleteMacroCommandHandler() = default;

void DeleteMacroCommandHandler::execute(
    const std::span<const std::string_view>& args,
    CommandResponseWriter& responseWriter,
    Arena& arena
) {
    auto& idArg = args[0];
    auto id = utils::strings::atol(idArg);

    macroStorage.remove(id);

    logger->debug("Deleted macro: %i", id);
}
