#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Register.h"

class RegisterManager {
public:
    RegisterManager();
    ~RegisterManager();
    RegisterManager(RegisterManager const&) = delete;

    void add(const std::string& name);
    std::shared_ptr<Register> get(const std::string& name);

private:
    std::vector<std::shared_ptr<Register>> registers;
};



