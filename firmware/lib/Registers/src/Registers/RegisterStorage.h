#pragma once

#include <array>
#include <mutex>

#include "RegisterDescriptor.h"

class RegisterStorage {
public:
    uint8_t read(const RegisterDescriptor& descriptor) const;
    void write(const RegisterDescriptor& descriptor, uint8_t value);

    const std::array<uint8_t, 32>& readAll() const;
    void writeAll(std::array<uint8_t, 32>& newValues);

private:
    std::array<uint8_t, 32> values{};
    mutable std::mutex lock;
};
