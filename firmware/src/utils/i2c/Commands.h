#pragma once

#include <utils/result.h>

namespace utils::i2c::commands {
        struct AnyParams {
        };

        template <class TParams>
        struct CommandDescriptor {
            const uint8_t id;
        };

        template <class TParams>
        class CommandHandler {
        public:
            explicit CommandHandler(uint8_t id) : id(id) {
            }

            virtual ~CommandHandler() = default;

            virtual void_result execute(const TParams* params) = 0;

            static TParams* parseData(uint8_t* message) {
                return reinterpret_cast<TParams*>(&message[1]); // 0 is the command ID
            }

            uint8_t id;
        };

        template <class TParams>
        class LambdaCommandHandler : public CommandHandler<TParams> {
        public:
            LambdaCommandHandler(
                uint8_t id,
                const std::function<void_result(const TParams& params)>& executeFn
            ) : CommandHandler<TParams>(id), executeFn(executeFn) {
            }

            ~LambdaCommandHandler() override = default;

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

            inline CommandDescriptor<SetEndpointParams> SetEndpoint = {.id = 0x01};
        }
    }
