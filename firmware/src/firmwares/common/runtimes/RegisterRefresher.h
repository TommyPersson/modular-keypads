#pragma once

#include <Registers/RegisterManager.h>

class RegisterRefresher {
public:
    explicit RegisterRefresher(utils::registers::RegisterManager& registers);
    virtual ~RegisterRefresher();

    virtual void setup() = 0;
    virtual void loop() = 0;

protected:
    utils::registers::RegisterManager& registers;
};
