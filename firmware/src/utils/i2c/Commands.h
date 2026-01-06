#pragma once

#include <functional>

#include "utils/result.h"

namespace utils::i2c::commands {
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

            virtual void_result execute(const TParams* params) = 0;

            static TParams* parseData(uint8_t* message) {
                return reinterpret_cast<TParams*>(&message[1]); // 0 is the command ID
            }

            const uint8_t id;
        };

        template <class TParams>
        class LambdaRemoteCommandHandler : public RemoteCommandHandler<TParams> {
        public:
            LambdaRemoteCommandHandler(
                uint8_t id,
                const std::function<void_result(const TParams& params)>& executeFn
            ) : RemoteCommandHandler<TParams>(id), executeFn(executeFn) {
            }

            ~LambdaRemoteCommandHandler() override = default;

            void_result execute(const TParams* params) override {
                return executeFn(*params);
            }

        private:
            std::function<void_result(const TParams& params)> executeFn;
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
