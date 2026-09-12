#include "CommandResponseWriter.h"

using namespace tfw::utils::commands;

CommandResponseWriter::CommandResponseWriter(const int commandId, tfw::hal::streams::OutputStream& outputStream) :
    commandId(commandId),
    sequenceNumber(1),
    outputStream(outputStream) {
}

CommandResponseWriter::~CommandResponseWriter() = default;
