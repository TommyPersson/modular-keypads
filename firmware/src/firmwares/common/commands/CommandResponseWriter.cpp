#include "CommandResponseWriter.h"

CommandResponseWriter::CommandResponseWriter(const int commandId, Print& outputStream) :
    commandId(commandId),
    sequenceNumber(1),
    outputStream(outputStream) {
}

CommandResponseWriter::~CommandResponseWriter() = default;

void CommandResponseWriter::writeLine(const std::string& line) {
    outputStream.printf("%%%i.%i:%s\n", commandId, sequenceNumber, line.c_str());
    sequenceNumber += 1;
}
