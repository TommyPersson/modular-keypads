#pragma once

#include "RegisterRefresher.h"

class NoOpRegisterRefresher final : public RegisterRefresher {
public:
    explicit NoOpRegisterRefresher(tfw::utils::registers::RegisterManager& registers);
    ~NoOpRegisterRefresher() override;

    void setup() override;
    void loop() override;
};
