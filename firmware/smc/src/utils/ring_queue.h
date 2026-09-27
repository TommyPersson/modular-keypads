#pragma once

#include <stdint.h>

namespace smc::utils {
    template <typename T, uint8_t N>
    class ring_queue {
        static_assert(N > 0, "ring_queue capacity must be greater than zero");

        public:
            ring_queue() = default;
            ~ring_queue() = default;

            void enqueue(T item) {
                buffer[producerIndex] = item;
                producerIndex = next(producerIndex);

                // If buffer is full, advance consumer to overwrite oldest item
                if (itemCount == N) {
                    consumerIndex = next(consumerIndex);
                } else {
                    itemCount++;
                }
            }

            T* dequeue() {
                if (itemCount > 0) {
                    const auto result = &buffer[consumerIndex];
                    consumerIndex = next(consumerIndex);
                    itemCount--;

                    return result;
                }

                return nullptr;
            }

            uint8_t size() const {
                return itemCount;
            }

            static constexpr uint8_t capacity() {
                return N;
            }

            template <typename Func>
            void forEach(Func callback) const {
                auto idx = consumerIndex;

                for (uint8_t i = 0; i < itemCount; ++i) {
                    callback(buffer[idx]);
                    idx = next(idx);
                }
            }

        private:
            // Wrap by comparison instead of modulo: AVR has no hardware divide
            static uint8_t next(const uint8_t index) {
                return index + 1 == N ? 0 : index + 1;
            }

            uint8_t consumerIndex = 0;
            uint8_t producerIndex = 0;
            uint8_t itemCount = 0;  // Track actual item count
            T buffer[N] = {};
    };
}
