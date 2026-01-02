#include "Logger.h"

Logger::Logger(
    std::optional<Print*>& outputStream,
    const std::string& name
    ) :
    outputStream(outputStream), name(name) {
}

namespace common::logging {
    namespace {
        std::optional<Print*> globalOutputStream;
    }

    void initialize(Print* outputStream1) {
        globalOutputStream = outputStream1;
    }

    std::shared_ptr<Logger> createLogger(const std::string& name) {
        return std::make_shared<Logger>(globalOutputStream, name);
    }
}
