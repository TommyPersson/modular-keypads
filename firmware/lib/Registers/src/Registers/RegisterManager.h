#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Register.h"
#include "RegisterDescriptor.h"
#include "RegisterStorage.h"

class RegisterManager {
public:
    RegisterManager();
    ~RegisterManager();
    RegisterManager(RegisterManager const&) = delete;

    std::shared_ptr<Register> configure(const RegisterDescriptor& descriptor);

    std::shared_ptr<Register> get(const RegisterDescriptor& descriptor) const;

    uint8_t read(const RegisterDescriptor& descriptor) const;
    uint8_t read(const std::string& descriptor) const;
    void write(const RegisterDescriptor& descriptor, uint8_t value);

    const std::vector<RegisterDescriptor>& list() const;

private:
    RegisterStorage storage;
    std::vector<std::shared_ptr<Register>> registers;
    std::vector<RegisterDescriptor> descriptors;
};



