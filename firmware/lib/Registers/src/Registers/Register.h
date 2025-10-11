#pragma once

#include <mutex>
#include <string>

class Register {
public:
    explicit Register(const std::string& name);
    ~Register();

    const std::string& getName() const { return this->name; };

    void write(uint8_t newValue);
    uint8_t read();

private:
    std::mutex lock;
    uint8_t value;
    std::string name;
};
