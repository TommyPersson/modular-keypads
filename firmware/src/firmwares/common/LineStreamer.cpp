#include "LineStreamer.h"

LineStreamer::LineStreamer(Stream& inputStream, size_t bufferSize) :
    inputStream(inputStream),
    bufferSize(bufferSize),
    lineBuffer(new char[bufferSize]),
    receiveBuffer(new char[bufferSize]),
    receiveBufferIndex(0),
    receiveBufferLength(0) {

    memset(this->lineBuffer, 0, this->bufferSize);
    memset(this->receiveBuffer, 0, this->bufferSize);
}

LineStreamer::~LineStreamer() {
    delete[] this->lineBuffer;
    delete[] this->receiveBuffer;
}

void LineStreamer::update() {
    if (receiveBufferLength <= 0) {
        populateReceiveBuffer();
    }

    while (this->receiveBufferLength > 0) {
        const auto& result = processReceiveBuffer();
        if (result != nullptr) {
            auto& value = *result;
            // TODO stack/pre-allocated strings instead
            this->lineSubject.notify(value);
        }
    }
}

std::shared_ptr<String> LineStreamer::processReceiveBuffer() {
    for (int i = 0; receiveBufferIndex < receiveBufferLength; receiveBufferIndex++, i++) {
        char c = this->receiveBuffer[receiveBufferIndex];
        if (c == '\n') {
            receiveBufferIndex += 1;
            auto line = std::make_shared<String>(this->lineBuffer);
            memset(this->lineBuffer, 0, this->bufferSize);
            return line;
        }

        this->lineBuffer[i] = c;
    }

    receiveBufferIndex = 0;
    receiveBufferLength = 0;

    return nullptr;
}

void LineStreamer::addObserver(Observer<String>* observer) {
    this->lineSubject.addObserver(observer);
}

void LineStreamer::removeObserver(Observer<String>* observer) {
    this->lineSubject.removeObserver(observer);
}

void LineStreamer::populateReceiveBuffer() {
    receiveBufferIndex = 0;
    receiveBufferLength = 0;

    int available = this->inputStream.available();
    if (available <= 0) {
        return;
    }

    const size_t numRead = this->inputStream.readBytes(this->receiveBuffer, available);
    if (numRead <= 0) {
        return;
    }

    receiveBufferLength = numRead;
}
