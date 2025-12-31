#include "SaveMacroCommandHandler.h"

#include <utils/strings.h>

using namespace common::macros;

namespace {
    auto logger = common::logging::createLogger("SaveMacroCommandHandler");

    std::shared_ptr<MacroData> createData(const std::span<const std::string_view>& args) {
        auto& idArg = args[0];
        auto id = utils::strings::atol(idArg);

        auto& typeArg = args[2];
        if (typeArg == "0x01") {
            auto& modifiersArg = args[3];
            auto& hidCodeArg = args[4];

            auto modifiers = utils::strings::atol(modifiersArg, 16);
            auto hidCode = utils::strings::atol(hidCodeArg, 16);

            return std::make_shared<ShortcutMacroData>(id, modifiers, hidCode);
        }

        if (typeArg == "0x02") {
            auto consumerControlCode = utils::strings::atou16(args[3], 16);

            return std::make_shared<ConsumerControlMacroData>(id, consumerControlCode);
        }

        // TODO support the sequence type

        logger->error("Unsupported macro type: %.*s", typeArg.length(), typeArg.data());

        return nullptr;
    }
}

SaveMacroCommandHandler::SaveMacroCommandHandler(MacroStorage& macroStorage)
    : CommandHandler("save.macro"),
      macroStorage(macroStorage) {
}

SaveMacroCommandHandler::~SaveMacroCommandHandler() = default;

void SaveMacroCommandHandler::execute(
    const std::span<const std::string_view>& args,
    CommandResponseWriter& responseWriter,
    Arena& arena
) {
    auto& nameArg = args[1];

    const Macro macro{
        .name = std::string(nameArg),
        .data = createData(args)
    };

    if (macro.data != nullptr) {
        macroStorage.write(macro);
    } else {
        logger->error("Unable to save macro.");
    }
}
