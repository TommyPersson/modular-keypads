#pragma once

#include <tfw/utils/result.h>

namespace tfw::hal::i2c::commands {
#pragma pack(push, 1)
    template <class TParams>
    struct CommandMessage {
        uint8_t commandId;
        uint64_t targetDeviceId;
        TParams params;
    };
#pragma pack(pop)

    struct AnyParams {
    };

    template <class TParams>
    struct RemoteCommandDescriptor {
        const uint8_t id;
    };

    template <class TParams>
    class RemoteCommandHandler {
    public:
        explicit RemoteCommandHandler(const uint8_t id) : id(id) {
        }

        virtual ~RemoteCommandHandler() = default;

        virtual utils::void_result execute(uint64_t targetDeviceId, const TParams* params) = 0;

        const CommandMessage<TParams>* parseMessage(const uint8_t* message) {
            auto parsedMessage = reinterpret_cast<const CommandMessage<TParams>*>(message);
            return parsedMessage;
        }

        const uint8_t id;
    };

    namespace builtin { // TODO separate the "slave port endpoint handling" from the more generic "command handling"?
#pragma pack(push, 1)
        struct SetEndpointParams {
            const uint8_t endpointId;
        };

        struct AcknowledgeDiscoveryParams {
        };

        struct AssignAddressParams {
            const uint8_t address;
        };

        struct RerollAddressParams {
        };
#pragma pack(pop)

        inline RemoteCommandDescriptor<SetEndpointParams> SetEndpoint = {.id = 0x01};

        // Releases the discovery line, once the master has found the device
        inline RemoteCommandDescriptor<AcknowledgeDiscoveryParams> AcknowledgeDiscovery = {.id = 0x02};

        // Moves the target device, and only it, from its temporary address to a permanent one
        inline RemoteCommandDescriptor<AssignAddressParams> AssignAddress = {.id = 0x03};

        // Makes every device at the address pick a new temporary address, sent when their identities collided. The
        // target device ID is ignored, as the colliding devices aren't known.
        inline RemoteCommandDescriptor<RerollAddressParams> RerollAddress = {.id = 0x04};
    }
}
