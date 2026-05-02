#pragma once

#include <tfw/utils/registers.h>

class RegisterRefresher {
public:
    explicit RegisterRefresher(tfw::utils::registers::RegisterManager& registers);
    virtual ~RegisterRefresher();

    virtual void setup() = 0;
    virtual void loop() = 0;

protected:
    tfw::utils::registers::RegisterManager& registers;
};
