#include "LineStreamer.h"

using namespace tfw::utils::streams;

LineStreamer::LineStreamer(tfw::hal::streams::InputStream& inputStream, size_t bufferSize) :
    inputStream(inputStream),
    bufferSize(bufferSize),
    lineBuffer(new char[bufferSize]) {
}

LineStreamer::~LineStreamer() {
    delete[] this->lineBuffer;
}

void LineStreamer::update() {
    // Limited to what is available now, so that a continuous stream can't keep the update from returning
    const auto available = this->inputStream.available();

    for (int i = 0; i < available; i++) {
        const int byte = this->inputStream.read();
        if (byte < 0) {
            break;
        }

        processCharacter(static_cast<char>(byte));
    }
}

void LineStreamer::processCharacter(const char c) {
    if (c == '\r') {
        return;
    }

    if (c == '\n') {
        if (!this->isDiscardingLine && this->lineLength > 0) {
            this->lineSubject.notify({.text = std::string_view(this->lineBuffer, this->lineLength)});
        }

        this->lineLength = 0;
        this->isDiscardingLine = false;
        return;
    }

    if (this->isDiscardingLine) {
        return;
    }

    // Lines that don't fit are discarded up to the next newline, rather than emitted truncated
    if (this->lineLength >= this->bufferSize) {
        this->lineLength = 0;
        this->isDiscardingLine = true;
        return;
    }

    this->lineBuffer[this->lineLength++] = c;
}

void LineStreamer::addObserver(observables::Observer<LineReceivedEvent>* observer) {
    this->lineSubject.addObserver(observer);
}

void LineStreamer::removeObserver(observables::Observer<LineReceivedEvent>* observer) {
    this->lineSubject.removeObserver(observer);
}
