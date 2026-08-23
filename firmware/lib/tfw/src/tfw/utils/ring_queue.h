#pragma once

#include <vector>

namespace tfw::utils {
    template <typename T>
    class ring_queue {
        public:
            explicit ring_queue(std::size_t size)
                : consumerIndex(0), producerIndex(0), itemCount(0), buffer(size) {
            }
            ~ring_queue() = default;

            void enqueue(T item) {
                buffer[producerIndex] = item;
                producerIndex = (producerIndex + 1) % buffer.size();

                // If buffer is full, advance consumer to overwrite oldest item
                if (itemCount == buffer.size()) {
                    consumerIndex = (consumerIndex + 1) % buffer.size();
                } else {
                    itemCount++;
                }
            }

            T* dequeue() {
                if (itemCount > 0) {
                    const auto result = &buffer[consumerIndex];
                    consumerIndex = (consumerIndex + 1) % buffer.size();
                    itemCount--;

                    return result;
                }

                return nullptr;
            }

            std::size_t size() const {
                return itemCount;
            }

            template <typename Func>
            void forEach(Func callback) const {
                const auto bufferSize = buffer.size();

                for (std::size_t i = 0; i < itemCount; ++i) {
                    const auto idx = (consumerIndex + i) % bufferSize;
                    callback(buffer[idx]);
                }
            }

        private:
            std::size_t consumerIndex;
            std::size_t producerIndex;
            std::size_t itemCount;  // Track actual item count
            std::vector<T> buffer;
    };
}
