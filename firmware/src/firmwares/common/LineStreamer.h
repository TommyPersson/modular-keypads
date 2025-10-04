#pragma once

#include <Stream.h>
#include <memory>

#include "utils/observables/Subject.h"
#include "utils/observables/Observer.h"

class LineStreamer {
public:
    explicit LineStreamer(Stream& inputStream, size_t bufferSize = 1024);
    ~LineStreamer();

    void addObserver(Observer<String>* observer);
    void removeObserver(Observer<String>* observer);

    void update();

private:
    std::shared_ptr<String> processReceiveBuffer();
    void populateReceiveBuffer();

    Subject<String> lineSubject;

    Stream& inputStream;
    const size_t bufferSize;
    char* lineBuffer;
    char* receiveBuffer;
    uint8_t receiveBufferIndex;
    uint8_t receiveBufferLength;
};
