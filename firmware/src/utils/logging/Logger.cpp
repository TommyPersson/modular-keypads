#include "Logger.h"

namespace utils::logging {
    Logger::Logger(
        std::optional<Print*>& outputStream,
        const std::string& name
    ) : outputStream(outputStream), name(name) {
    }

    namespace {
        std::optional<Print*> globalOutputStream;
    }

    void initialize(Print* outputStream) {
        globalOutputStream = outputStream;
    }

    std::shared_ptr<Logger> createLogger(const std::string& name) {
        return std::make_shared<Logger>(globalOutputStream, name);
    }
}
