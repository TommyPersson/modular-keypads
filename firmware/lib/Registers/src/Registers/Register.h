#pragma once

#include <string>

#include "RegisterDescriptor.h"
#include "RegisterStorage.h"

class Register {
public:
    explicit Register(const RegisterDescriptor& descriptor, RegisterStorage& storage);
    ~Register();

    const std::string& getName() const { return this->descriptor.name; }
    const RegisterDescriptor& getDescriptor() const { return this->descriptor; }

    void write(uint8_t newValue) const;
    uint8_t read() const;


private:
    const RegisterDescriptor descriptor;
    RegisterStorage& storage;
};
