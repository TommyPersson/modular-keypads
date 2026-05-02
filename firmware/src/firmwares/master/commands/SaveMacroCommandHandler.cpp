#include "SaveMacroCommandHandler.h"

#include "firmwares/common/macros/MacroDataSerializers.h"
#include <tfw/utils/strings.h>
#include <tfw/hal/logging.h>

using namespace common::macros;

namespace {
    auto logger = tfw::utils::logging::createLogger("SaveMacroCommandHandler");

    std::shared_ptr<MacroData> createData(const std::span<const std::string_view>& args, tfw::utils::allocations::Arena& arena) {
        auto& macroIdArg = args[0];
        auto macroId = tfw::utils::strings::atol(macroIdArg);
        auto& typeArg = args[2];
        auto type = static_cast<MacroType>(tfw::utils::strings::atol(typeArg, 16));

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

tfw::utils::void_result SaveMacroCommandHandler::execute(
    const std::span<const std::string_view>& args,
    tfw::utils::commands::CommandResponseWriter& responseWriter,
    tfw::utils::allocations::Arena& arena
) {
    auto& nameArg = args[1];

    const Macro macro{
        .name = nameArg,
        .data = createData(args, arena)
    };

    if (macro.data != nullptr) {
        macroStorage.write(macro);
    } else {
        return tfw::utils::void_result::error("unable.to.save.macro");
    }

    return tfw::utils::void_result::success();
}
