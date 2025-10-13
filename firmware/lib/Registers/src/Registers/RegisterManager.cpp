#include "RegisterManager.h"

RegisterManager::RegisterManager() = default;

RegisterManager::~RegisterManager() = default;

std::shared_ptr<Register> RegisterManager::add(const std::string& name) {
    auto reg = std::make_shared<Register>(name);
    this->registers.emplace_back(reg);
    return reg;
}

std::shared_ptr<Register> RegisterManager::get(const std::string& name) {
    for (auto& reg : this->registers) {
        if (reg->getName() == name) {
            return reg;
        }
    }

    return nullptr;
}
