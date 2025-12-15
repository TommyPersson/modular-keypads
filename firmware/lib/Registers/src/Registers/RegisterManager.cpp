#include "RegisterManager.h"

RegisterManager::RegisterManager() = default;

RegisterManager::~RegisterManager() = default;

std::shared_ptr<Register> RegisterManager::configure(const RegisterDescriptor& descriptor) {
    auto reg = std::make_shared<Register>(descriptor, storage);
    this->registers.emplace_back(reg);
    this->descriptors.emplace_back(descriptor);
    return reg;
}

std::shared_ptr<Register> RegisterManager::get(const RegisterDescriptor& descriptor) const {
    for (const auto& reg : this->registers) {
        if (reg->getName() == descriptor.name) {
            return reg;
        }
    }

    return nullptr;
}

uint8_t RegisterManager::read(const RegisterDescriptor& descriptor) const {
    return storage.read(descriptor);
}

uint8_t RegisterManager::read(const std::string& name) const {
    for (const auto& descriptor : this->descriptors) {
        if (descriptor.name == name) {
            return read(descriptor);
        }
    }

    return 0;
}

const std::array<uint8_t, 32>& RegisterManager::readAll() const {
    return storage.readAll();
}

void RegisterManager::write(const RegisterDescriptor& descriptor, uint8_t value) {
    storage.write(descriptor, value);
}

const std::vector<RegisterDescriptor>& RegisterManager::list() const {
    return descriptors;
}

void RegisterManager::writeAll(std::span<uint8_t, 32>& data) {
    storage.writeAll(data);
}
