#include "RegisterManager.h"

RegisterManager::RegisterManager() = default;

RegisterManager::~RegisterManager() = default;

void RegisterManager::add(const std::string& name) {
    this->registers.emplace_back(std::make_shared<Register>(name));
}

std::shared_ptr<Register> RegisterManager::get(const std::string& name) {
    for (auto& reg : this->registers) {
        if (reg->getName() == name) {
            return reg;
        }
    }

    return nullptr;
}
