#pragma once

#include <vector>

namespace tfw::utils {
    template <typename T>
    class ring_queue {
        public:
            explicit ring_queue(std::size_t size) : consumerIndex(0), producerIndex(0), buffer(size) {
            }
            ~ring_queue() = default;

            void enqueue(T item) {
                buffer[producerIndex] = item;
                producerIndex = (producerIndex + 1) % buffer.size();

                // If producer caught up to consumer, advance consumer
                if (producerIndex == consumerIndex) {
                    consumerIndex = (producerIndex + 1) % buffer.size();
                }
            }

            T* dequeue() {
                if (consumerIndex != producerIndex) {
                    const auto result = &buffer[consumerIndex];
                    consumerIndex = (consumerIndex + 1) % buffer.size();

                    return result;
                }

                return nullptr;
            }

        private:
            std::size_t consumerIndex;
            std::size_t producerIndex;
            std::vector<T> buffer;
    };
}
