#include "Register.h"

#include "RegisterManager.h"

namespace utils::registers {
    Register::Register(const RegisterDescriptor& descriptor, RegisterStorage& storage) :
        descriptor(descriptor),
        storage(storage) {
    }

    Register::~Register() = default;

    void Register::write(const uint8_t newValue) const {
        storage.write(descriptor, newValue);
    }

    uint8_t Register::read() const {
        return storage.read(descriptor);
    }
}