#pragma once

#include <memory>
#include <cstdint>
#include <string_view>

#include "tfw/hal/streams/InputStream.h"
#include "tfw/utils/observables.h"

namespace tfw::utils::streams {
    struct LineReceivedEvent {
        std::string_view text;
    };

    class LineStreamer {
    public:
        explicit LineStreamer(tfw::hal::streams::InputStream& inputStream, size_t bufferSize = 1024);
        ~LineStreamer();

        void addObserver(observables::Observer<LineReceivedEvent>* observer);
        void removeObserver(observables::Observer<LineReceivedEvent>* observer);

        void update();

    private:
        std::string_view processReceiveBuffer();
        void populateReceiveBuffer();

        observables::Subject<LineReceivedEvent> lineSubject;

        tfw::hal::streams::InputStream& inputStream;
        const size_t bufferSize;
        char* lineBuffer;
        char* receiveBuffer;
        uint8_t receiveBufferIndex;
        uint8_t receiveBufferLength;
    };
}
