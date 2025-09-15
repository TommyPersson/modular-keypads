#pragma once

#include <mutex>

class Register {
public:
    explicit Register(const char* name);
    ~Register();

    char* getName() const { return this->name; };

    void write(uint8_t newValue);
    uint8_t read();

private:
    std::mutex lock;
    uint8_t value;
    char* name;
};
