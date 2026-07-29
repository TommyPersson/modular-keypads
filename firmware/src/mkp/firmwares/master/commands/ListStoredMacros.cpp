#include "ListStoredMacros.h"

#include "firmwares/common/macros/MacroDataSerializers.h"
#include <tfw/utils/strings.h>
#include <tfw/hal/logging.h>

using namespace common::macros;
using namespace mkp::firmwares::master::commands;

namespace {
    auto logger = tfw::hal::logging::createLogger("ListStoredMacrosCommandHandler");
}

ListStoredMacros::ListStoredMacros(MacroStorage& macroStorage)
    : CommandHandler("list.stored.macros"),
      macroStorage(macroStorage) {
}

ListStoredMacros::~ListStoredMacros() = default;

tfw::utils::void_result ListStoredMacros::execute(
    const std::span<const std::string_view>& args,
    tfw::utils::commands::CommandResponseWriter& responseWriter,
    tfw::utils::allocations::Arena& arena
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

    return tfw::utils::void_result::success();
}
