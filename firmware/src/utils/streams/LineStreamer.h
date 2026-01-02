#pragma once

#include <memory>
#include <Stream.h>

#include "utils/observables/Observer.h"
#include "utils/observables/Subject.h"

namespace utils::streams {
    struct LineReceivedEvent {
        std::string_view text;
    };

    class LineStreamer {
    public:
        explicit LineStreamer(Stream& inputStream, size_t bufferSize = 1024);
        ~LineStreamer();

        void addObserver(observables::Observer<LineReceivedEvent>* observer);
        void removeObserver(observables::Observer<LineReceivedEvent>* observer);

        void update();

    private:
        std::string_view processReceiveBuffer();
        void populateReceiveBuffer();

        observables::Subject<LineReceivedEvent> lineSubject;

        Stream& inputStream;
        const size_t bufferSize;
        char* lineBuffer;
        char* receiveBuffer;
        uint8_t receiveBufferIndex;
        uint8_t receiveBufferLength;
    };
}
