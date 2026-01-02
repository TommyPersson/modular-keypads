#include "CommandResponseWriter.h"

using namespace utils::commands;

CommandResponseWriter::CommandResponseWriter(const int commandId, Print& outputStream) :
    commandId(commandId),
    sequenceNumber(1),
    outputStream(outputStream) {
}

CommandResponseWriter::~CommandResponseWriter() = default;
