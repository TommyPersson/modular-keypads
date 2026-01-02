#include "SaveMacroCommandHandler.h"

#include "firmwares/common/macros/MacroDataSerializers.h"
#include "utils/strings.h"
#include "utils/logging/Logger.h"

using namespace common::macros;

namespace {
    auto logger = utils::logging::createLogger("SaveMacroCommandHandler");

    std::shared_ptr<MacroData> createData(const std::span<const std::string_view>& args, utils::allocations::Arena& arena) {
        auto& macroIdArg = args[0];
        auto macroId = utils::strings::atol(macroIdArg);
        auto& typeArg = args[2];
        auto type = static_cast<MacroType>(utils::strings::atol(typeArg, 16));

        for (auto serializer : macroDataSerializers) {
            auto typedSerializer = static_cast<MacroDataStorageSerializer<MacroData>*>(serializer);
            if (typedSerializer->handles(type)) {
                return typedSerializer->deserialize(macroId, args.subspan(3), arena);
            }
        }

        logger->error("Unsupported macro type: %.*s", typeArg.length(), typeArg.data());

        return nullptr;
    }
}

SaveMacroCommandHandler::SaveMacroCommandHandler(MacroStorage& macroStorage)
    : CommandHandler("save.macro"),
      macroStorage(macroStorage) {
}

SaveMacroCommandHandler::~SaveMacroCommandHandler() = default;

utils::void_result SaveMacroCommandHandler::execute(
    const std::span<const std::string_view>& args,
    utils::commands::CommandResponseWriter& responseWriter,
    utils::allocations::Arena& arena
) {
    auto& nameArg = args[1];

    const Macro macro{
        .name = std::string(nameArg),
        .data = createData(args, arena)
    };

    if (macro.data != nullptr) {
        macroStorage.write(macro);
    } else {
        return utils::void_result::error("unable.to.save.macro");
    }

    return utils::void_result::success();
}
