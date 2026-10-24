#include "WriteFirmwareUpdate.h"

#include <mbedtls/base64.h>

#include <tfw/utils/strings.h>

namespace {
    // Commands are limited to a 1024 character line, so chunks are sent as at most 512 bytes, 684 in Base64
    constexpr size_t MAX_CHUNK_SIZE = 512;
}

using namespace mkp::firmwares::master::commands;

WriteFirmwareUpdate::WriteFirmwareUpdate(
    mkp::components::firmware::FirmwareUpdater& firmwareUpdater
) : CommandHandler("write.firmware.update"),
    firmwareUpdater(firmwareUpdater) {
}

WriteFirmwareUpdate::~WriteFirmwareUpdate() = default;

tfw::utils::void_result WriteFirmwareUpdate::execute(
    const std::span<const std::string_view>& args,
    tfw::utils::commands::CommandResponseWriter& responseWriter,
    tfw::utils::allocations::Arena& arena
) {
    if (args.size() < 2) {
        return tfw::utils::void_result::error("missing.firmware.chunk");
    }

    const auto offset = tfw::utils::strings::atou32(args[0]);
    const auto& encoded = args[1];

    uint8_t chunk[MAX_CHUNK_SIZE];
    size_t chunkSize = 0;
    const auto decodeResult = mbedtls_base64_decode(
        chunk,
        sizeof(chunk),
        &chunkSize,
        reinterpret_cast<const unsigned char*>(encoded.data()),
        encoded.size()
    );

    if (decodeResult != 0) {
        return tfw::utils::void_result::error("invalid.firmware.chunk");
    }

    return firmwareUpdater.write(offset, std::span(chunk, chunkSize));
}
