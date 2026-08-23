#pragma once

// <expected> is not available in this compiler version, so we just do something simple for now.

#include <cstdint>
#include <cstdio>
#include <string>
#include <memory>
#include <utility>

#include "allocations/Arena.h"

namespace tfw::utils {
    template <typename T>
    struct result {
        bool has_error = false;

        union {
            T value;
            const char* error_code;
        };

        result() : has_error(true), error_code("") {
        }

        result(const result& other) : has_error(other.has_error) {
            if (has_error) {
                error_code = other.error_code;
            } else {
                new(std::addressof(value)) T(other.value);
            }
        }

        result(result&& other) noexcept(std::is_nothrow_move_constructible_v<T>) : has_error(other.has_error) {
            if (has_error) {
                error_code = other.error_code;
            } else {
                new(std::addressof(value)) T(std::move(other.value));
            }
        }

        result& operator=(const result& other) {
            if (this == &other) {
                return *this;
            }
            destroy_value();
            has_error = other.has_error;
            if (has_error) {
                error_code = other.error_code;
            } else {
                new(std::addressof(value)) T(other.value);
            }
            return *this;
        }

        result& operator=(result&& other) noexcept(std::is_nothrow_move_constructible_v<T>) {
            if (this == &other) {
                return *this;
            }
            destroy_value();
            has_error = other.has_error;
            if (has_error) {
                error_code = other.error_code;
            } else {
                new(std::addressof(value)) T(std::move(other.value));
            }
            return *this;
        }

        ~result() {
            destroy_value();
        }

        static result of(T value) {
            result r;
            r.has_error = false;
            new(std::addressof(r.value)) T(std::move(value));
            return r;
        }

        static result error(const char* error_code) { // TODO accept format string?
            result r;
            r.has_error = true;
            r.error_code = error_code;
            return r;
        }

    private:
        void destroy_value() {
            if (!has_error) {
                value.~T();
            }
        }
    };

    struct void_result {
        bool has_error = false;

        const char* error_code = "";

        constexpr static void_result success() {
            return void_result{};
        }

        static void_result error(const char* error_code) {// TODO accept format string?
            return void_result{.has_error = true, .error_code = error_code};
        }

        // New method that accepts a format string and uses arena allocator
        template<typename... Args>
        static void_result errorf(allocations::Arena& arena, const char* format, Args... args) {
            const auto buffer_size = static_cast<size_t>(std::snprintf(nullptr, 0, format, args...) + 1);
            const auto buffer = reinterpret_cast<char*>(arena.allocate(buffer_size));
            std::snprintf(buffer, buffer_size, format, args...);
            return void_result{.has_error = true, .error_code = buffer};
        }
    };
}
