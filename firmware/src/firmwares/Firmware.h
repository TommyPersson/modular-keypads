#pragma once

#include <memory>

class Firmware {
public:
    virtual ~Firmware() = default;
    virtual void setup() = 0;
    virtual void loop() = 0;

    static std::unique_ptr<Firmware> create(char variant);
};
