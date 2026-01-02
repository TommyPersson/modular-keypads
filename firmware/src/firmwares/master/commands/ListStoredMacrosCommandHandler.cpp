#include "ListStoredMacrosCommandHandler.h"

#include "firmwares/common/macros/MacroDataSerializers.h"
#include "utils/strings.h"
#include "utils/logging/Logger.h"

using namespace common::macros;

namespace {
    auto logger = utils::logging::createLogger("ListStoredMacrosCommandHandler");
}

ListStoredMacrosCommandHandler::ListStoredMacrosCommandHandler(MacroStorage& macroStorage)
    : CommandHandler("list.stored.macros"),
      macroStorage(macroStorage) {
}

ListStoredMacrosCommandHandler::~ListStoredMacrosCommandHandler() = default;

utils::void_result ListStoredMacrosCommandHandler::execute(
    const std::span<const std::string_view>& args,
    utils::commands::CommandResponseWriter& responseWriter,
    utils::allocations::Arena& arena
) {
    macroStorage.forEach(
        [&](const Macro& macro) {
            std::string_view dataPart;

            for (auto serializer : macroDataSerializers) {
                auto typedSerializer = static_cast<MacroDataStorageSerializer<MacroData>*>(serializer);
                if (typedSerializer->handles(macro.data->type)) {
                    dataPart = typedSerializer->serialize(*macro.data, arena);
                }
            }

            if (!dataPart.empty()) {
                responseWriter.writeLineF(
                    "%.*s=0x%04x:0x%02x:%.*s",
                    macro.name.length(),
                    macro.name.data(),
                    macro.data->id,
                    macro.data->type,
                    dataPart.length(),
                    dataPart.data()
                );
            }
        }
    );

    return utils::void_result::success();
}
