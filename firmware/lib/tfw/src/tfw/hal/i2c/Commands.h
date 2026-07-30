#pragma once

#include <functional>

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

        virtual utils::void_result execute(const TParams* params) = 0;

        const CommandMessage<TParams>* parseMessage(const uint8_t* message) {
            auto parsedMessage = reinterpret_cast<const CommandMessage<TParams>*>(message);
            return parsedMessage;
        }

        const uint8_t id;
    };

    template <class TParams>
    class LambdaRemoteCommandHandler : public RemoteCommandHandler<TParams> {
    public:
        LambdaRemoteCommandHandler(
            uint8_t id,
            const std::function<utils::void_result(const TParams& params)>& executeFn
        ) : RemoteCommandHandler<TParams>(id),
            executeFn(executeFn) {
        }

        ~LambdaRemoteCommandHandler() override = default;

        utils::void_result execute(const TParams* params) override {
            return executeFn(*params);
        }

    private:
        std::function<utils::void_result(const TParams& params)> executeFn;
    };

    namespace builtin { // TODO separate the "slave port endpoint handling" from the more generic "command handling"?
#pragma pack(push, 1)
        struct SetEndpointParams {
            const uint8_t endpointId;
        };
#pragma pack(pop)

        inline RemoteCommandDescriptor<SetEndpointParams> SetEndpoint = {.id = 0x01};
    }
}
