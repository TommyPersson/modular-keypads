#include "Logger.h"

#ifdef ESP32
#include <esp_log_write.h>
#endif

namespace tfw::hal::logging {
    Logger::Logger(
        std::optional<tfw::hal::streams::OutputStream*>& outputStream,
        const std::string& name
    ) : outputStream(outputStream), name(name) {
    }

    namespace {
        std::optional<tfw::hal::streams::OutputStream*> globalOutputStream;

#ifdef ESP32
        int esp_vprintf(const char* format, va_list args) {
            const auto ostream = globalOutputStream.value_or(nullptr);
            if (ostream == nullptr) {
                return 0;
            }

            size_t numPrinted = 0;

            numPrinted += ostream->print("$");
            numPrinted += ostream->print("esp");
            numPrinted += ostream->print(":");
            numPrinted += ostream->vprintf(format, args);

            ostream->flush();

            return numPrinted;
        }
#endif
    }

    void initialize(tfw::hal::streams::OutputStream* outputStream) {
        globalOutputStream = outputStream;

#ifdef ESP32
        esp_log_set_vprintf(esp_vprintf);
#endif
    }

    std::shared_ptr<Logger> createLogger(const std::string& name) {
        return std::make_shared<Logger>(globalOutputStream, name);
    }
}
