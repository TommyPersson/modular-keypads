#include "CommandResponseWriter.h"

CommandResponseWriter::CommandResponseWriter(const int commandId, Print& outputStream) :
    commandId(commandId),
    sequenceNumber(1),
    outputStream(outputStream) {}

CommandResponseWriter::~CommandResponseWriter() = default;
