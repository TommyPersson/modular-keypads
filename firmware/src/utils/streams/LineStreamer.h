#pragma once

#include <Stream.h>
#include <memory>

#include "utils/observables/Subject.h"
#include "utils/observables/Observer.h"

struct LineEvent {
    std::string text;
};

class LineStreamer {
public:
    explicit LineStreamer(Stream& inputStream, size_t bufferSize = 1024);
    ~LineStreamer();

    void addObserver(Observer<LineEvent>* observer);
    void removeObserver(Observer<LineEvent>* observer);

    void update();

private:
    std::shared_ptr<std::string> processReceiveBuffer();
    void populateReceiveBuffer();

    Subject<LineEvent> lineSubject;

    Stream& inputStream;
    const size_t bufferSize;
    char* lineBuffer;
    char* receiveBuffer;
    uint8_t receiveBufferIndex;
    uint8_t receiveBufferLength;
};
