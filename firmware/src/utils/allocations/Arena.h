#pragma once

#include <vector>
#include <list>
#include <cstring>

#include "firmwares/common/logging/Logger.h"

class Arena {
public:
    explicit Arena(const size_t size)
        : size(size) {
        buffer = new uint8_t[size];
        memset(buffer, 0, size);
        offset = 0;
    }

    ~Arena() {
        delete[] buffer;
    }

    uint8_t* allocate(size_t desiredSize) {
        const size_t remainder = size - offset;

        if (desiredSize > remainder) {
            return nullptr;
        }

        auto pointer = &buffer[offset];

        offset += desiredSize;

        return pointer;
    }

    void reset() {
        std::memset(buffer, 0, size);
        offset = 0;
    }

    uint8_t* getBuffer() const {
        return this->buffer;
    }

private:
    size_t size;
    size_t offset;
    uint8_t* buffer;
};

template <class T>
struct ArenaAllocator {
    using value_type = T;
    using size_type = std::size_t;
    using propagate_on_container_move_assignment = std::true_type;

    ArenaAllocator(Arena& arena, Logger& logger) noexcept
        : arena(&arena),
          logger(&logger) {
    }

    ArenaAllocator(const ArenaAllocator& other) noexcept
        : arena(other.arena),
          logger(other.logger) {
    }

    template <class U>
    ArenaAllocator(const ArenaAllocator<U>& other) noexcept
        : arena(other.arena),
          logger(other.logger) {
    }

    ArenaAllocator(ArenaAllocator&& other) noexcept
        : arena(other.arena),
          logger(other.logger) {
    }

    ArenaAllocator& operator =(const ArenaAllocator&) noexcept {
        // noop
        return *this;
    }

    ArenaAllocator& operator =(ArenaAllocator&& other) noexcept {
        arena = other.arena;
        logger = other.logger;
        return *this;
    }

    T* allocate(std::size_t n) {
        logger->debug("allocate(%i), sizeOf(T)=%i", n, sizeof(T));
        return reinterpret_cast<T*>(arena->allocate(n * sizeof(T)));
    }

    void deallocate(T* p, std::size_t n) {
        //logger->debug("deallocate(%i)", n);
    }

    Arena* arena;
    Logger* logger;
};


template <class T, class U>
constexpr bool operator==(const ArenaAllocator<T>&, const ArenaAllocator<U>&) noexcept {
    return true;
}

template <class T, class U>
constexpr bool operator!=(const ArenaAllocator<T>&, const ArenaAllocator<U>&) noexcept {
    return false;
}

namespace arena {
    template <typename T>
    using vector = std::vector<T, ArenaAllocator<T>>;

    template <typename T>
    using list = std::list<T, ArenaAllocator<T>>;
}
