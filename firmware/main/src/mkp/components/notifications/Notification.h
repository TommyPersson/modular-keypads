#pragma once

#include <string>
#include <vector>

namespace mkp::components::notifications {
    struct Notification {
        std::string type;
        std::vector<std::string> args;
    };
}