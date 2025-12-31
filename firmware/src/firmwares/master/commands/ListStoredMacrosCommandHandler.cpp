#include "ListStoredMacrosCommandHandler.h"

#include <utils/strings.h>

using namespace common::macros;

namespace {
    auto logger = common::logging::createLogger("ListStoredMacrosCommandHandler");
}

ListStoredMacrosCommandHandler::ListStoredMacrosCommandHandler(MacroStorage& macroStorage)
    : CommandHandler("list.stored.macros"),
      macroStorage(macroStorage) {
}

ListStoredMacrosCommandHandler::~ListStoredMacrosCommandHandler() = default;

void ListStoredMacrosCommandHandler::execute(
    const std::span<const std::string_view>& args,
    CommandResponseWriter& responseWriter,
    Arena& arena
) {
    macroStorage.forEach(
        [&](const Macro& macro) {
            if (macro.data->type == MacroType::SHORTCUT) {
                const auto& data = std::dynamic_pointer_cast<ShortcutMacroData>(macro.data);
                responseWriter.writeLineF(
                    "%s=0x%04x:0x%02x:0x%02x:0x%02x",
                    macro.name.c_str(),
                    data->id,
                    data->type,
                    data->modifiers,
                    data->hidKeyCode
                );
            }

            if (macro.data->type == MacroType::CONSUMER_CONTROL) {
                const auto& data = std::dynamic_pointer_cast<ConsumerControlMacroData>(macro.data);
                responseWriter.writeLineF(
                    "%s=0x%04x:0x%02x:0x%04x",
                    macro.name.c_str(),
                    data->id,
                    data->type,
                    data->usageId
                );
            }
        }
    );
}
