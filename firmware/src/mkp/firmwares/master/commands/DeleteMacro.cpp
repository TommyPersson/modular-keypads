#include "DeleteMacro.h"

#include <tfw/utils/strings.h>
#include <tfw/hal/logging.h>

using namespace common::macros;
using namespace mkp::firmwares::master::commands;

namespace {
    auto logger = tfw::hal::logging::createLogger("DeleteMacroCommandHandler");
}

DeleteMacro::DeleteMacro(MacroStorage& macroStorage)
    : CommandHandler("delete.macro"),
      macroStorage(macroStorage) {
}

DeleteMacro::~DeleteMacro() = default;

tfw::utils::void_result DeleteMacro::execute(
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
