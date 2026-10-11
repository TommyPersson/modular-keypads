#include "SaveMacro.h"

#include "mkp/components/macros/MacroDataSerializers.h"
#include <tfw/utils/strings.h>
#include <tfw/hal/logging.h>

using namespace mkp::components::macros;
using namespace mkp::firmwares::master::commands;

namespace {
    auto logger = tfw::hal::logging::createLogger("SaveMacroCommandHandler");

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

SaveMacro::SaveMacro(MacroStorage& macroStorage)
    : CommandHandler("save.macro"),
      macroStorage(macroStorage) {
}

SaveMacro::~SaveMacro() = default;

tfw::utils::void_result SaveMacro::execute(
    const std::span<const std::string_view>& args,
    tfw::utils::commands::CommandResponseWriter& responseWriter,
    tfw::utils::allocations::Arena& arena
) {
    // Macro ID, name and type, followed by the data of the type
    if (args.size() < 3) {
        return tfw::utils::void_result::error("unable.to.save.macro");
    }

    auto& nameArg = args[1];

    const Macro macro{
        .name = nameArg,
        .data = createData(args, arena)
    };

    if (macro.data == nullptr || macroStorage.write(macro) < 0) {
        return tfw::utils::void_result::error("unable.to.save.macro");
    }

    // The storage assigns new macros their ID while writing them, which the caller needs to refer to them
    responseWriter.writeLineF("%u", static_cast<unsigned>(macro.data->id));

    return tfw::utils::void_result::success();
}
