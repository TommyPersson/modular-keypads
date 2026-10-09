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
        void processCharacter(char c);

        observables::Subject<LineReceivedEvent> lineSubject;

        tfw::hal::streams::InputStream& inputStream;
        const size_t bufferSize;

        // Kept across updates, as lines can arrive over several reads
        char* lineBuffer;
        size_t lineLength = 0;
        bool isDiscardingLine = false;
    };
}
