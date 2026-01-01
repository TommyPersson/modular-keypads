#pragma once

#include <variant>

namespace utils {

    template <typename T>
    struct result {
        bool has_error = false;

        union {
            T value;
            const char* error_code = "";
        };

        static result of(T value) {
            return result{.value = value};
        }

        static result error(std::uint32_t error_code) {
            return result{.error_code = error_code, .has_error = true};
        }
    };

    struct void_result {
        bool has_error = false;

        const char* error_code = "";

        constexpr static void_result success()  {
            return void_result{};
        }

        static void_result error(const char* error_code) {
            return void_result{.has_error = true, .error_code = error_code};
        }
    };
}
