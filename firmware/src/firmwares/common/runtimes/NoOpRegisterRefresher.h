#pragma once

#include "RegisterRefresher.h"

class NoOpRegisterRefresher final : public RegisterRefresher {
public:
    explicit NoOpRegisterRefresher(utils::registers::RegisterManager& registers);
    ~NoOpRegisterRefresher() override;

    void begin() override;
    void loop() override;
};
