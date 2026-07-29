#pragma once

#include <string>
#include <vector>

struct Notification {
    std::string type;
    std::vector<std::string> args;
};