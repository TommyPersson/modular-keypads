#pragma once

#include <Print.h>
#include <string>

class CommandResponseWriter {
public:
    CommandResponseWriter(int commandId, Print& outputStream);
    ~CommandResponseWriter();

    void writeLine(const std::string& line);

private:
    const int commandId;
    int sequenceNumber;
    Print& outputStream;
};
