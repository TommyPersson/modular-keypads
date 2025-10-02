#pragma once

#include <Stream.h>
#include <memory>

class LineReader {
public:
    explicit LineReader(Stream& inputStream, size_t bufferSize = 1024);
    ~LineReader();

    std::shared_ptr<String> readLine();

private:
    std::shared_ptr<String> processReceiveBuffer();
    void populateReceiveBuffer();

    Stream& inputStream;
    const size_t bufferSize;
    char* lineBuffer;
    char* receiveBuffer;
    uint8_t receiveBufferIndex;
    uint8_t receiveBufferLength;
};
