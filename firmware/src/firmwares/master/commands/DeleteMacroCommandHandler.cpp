#include "DeleteMacroCommandHandler.h"

#include "../../../tfw/utils/strings.h"
#include <tfw/hal/logging.h>

using namespace common::macros;

namespace {
    auto logger = tfw::utils::logging::createLogger("DeleteMacroCommandHandler");
}

DeleteMacroCommandHandler::DeleteMacroCommandHandler(MacroStorage& macroStorage)
    : CommandHandler("delete.macro"),
      macroStorage(macroStorage) {
}

DeleteMacroCommandHandler::~DeleteMacroCommandHandler() = default;

tfw::utils::void_result DeleteMacroCommandHandler::execute(
    const std::span<const std::string_view>& args,
    tfw::utils::commands::CommandResponseWriter& responseWriter,
    tfw::utils::allocations::Arena& arena
) {
    auto& idArg = args[0];
    auto id = tfw::utils::strings::atol(idArg);

    macroStorage.remove(id);

    logger->debug("Deleted macro: %i", id);

    return tfw::utils::void_result::success();
}
