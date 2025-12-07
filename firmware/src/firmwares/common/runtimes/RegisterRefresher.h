#pragma once
#include <Registers/RegisterManager.h>

class RegisterRefresher {
public:
    explicit RegisterRefresher(RegisterManager& registers);
    virtual ~RegisterRefresher();

    virtual void begin() = 0;
    virtual void loop() = 0;

protected:
    RegisterManager& registers;
};
