#pragma once

class Arena {
public:
    explicit Arena(const size_t size) : size(size) {
        buffer = std::make_unique<uint8_t[]>(size);
        offset = 0;
    }

    uint8_t* allocate(size_t desiredSize) {
        const size_t remainder = size - offset;

        if (desiredSize > remainder) {
            return nullptr;
        }

        offset += desiredSize;

        return &buffer[offset];
    }

    void reset() {
        offset = 0;
    }
private:
    size_t size;
    size_t offset;
    std::unique_ptr<uint8_t[]> buffer;
};