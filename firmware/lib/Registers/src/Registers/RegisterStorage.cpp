#include "RegisterStorage.h"

#include <span>

uint8_t RegisterStorage::read(const RegisterDescriptor& descriptor) const {
    std::lock_guard guard(lock);

    return values.at(descriptor.index);
}

void RegisterStorage::write(const RegisterDescriptor& descriptor, uint8_t value) {
    std::lock_guard guard(lock);

    values.at(descriptor.index) = value;
}

const std::array<uint8_t, 32>& RegisterStorage::readAll() const {
    std::lock_guard guard(lock);

    return values;
}

void RegisterStorage::writeAll(std::span<uint8_t, 32>& newValues) {
    std::lock_guard guard(lock);

    std::copy(newValues.begin(), newValues.end(), values.begin());
}
