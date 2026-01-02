#include "DeleteMacroCommandHandler.h"

#include "utils/strings.h"
#include "utils/logging/Logger.h"

using namespace common::macros;

namespace {
    auto logger = utils::logging::createLogger("DeleteMacroCommandHandler");
}

DeleteMacroCommandHandler::DeleteMacroCommandHandler(MacroStorage& macroStorage)
    : CommandHandler("delete.macro"),
      macroStorage(macroStorage) {
}

DeleteMacroCommandHandler::~DeleteMacroCommandHandler() = default;

utils::void_result DeleteMacroCommandHandler::execute(
    const std::span<const std::string_view>& args,
    utils::commands::CommandResponseWriter& responseWriter,
    utils::allocations::Arena& arena
) {
    auto& idArg = args[0];
    auto id = utils::strings::atol(idArg);

    macroStorage.remove(id);

    logger->debug("Deleted macro: %i", id);

    return utils::void_result::success();
}
