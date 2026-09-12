#include <gtest/gtest.h>
#include <vector>
#include "tfw/utils/ring_queue.h"

using namespace tfw::utils;

// Test fixture for ring_queue tests
class RingQueueTest : public ::testing::Test {
protected:
    static constexpr size_t SMALL_SIZE = 3;
    static constexpr size_t MEDIUM_SIZE = 10;
    static constexpr size_t LARGE_SIZE = 1000;
};

// Basic construction and initial state
TEST_F(RingQueueTest, ConstructionWithSize) {
    ring_queue<int> q(SMALL_SIZE);
    // Should construct without error
    EXPECT_TRUE(true);
}

TEST_F(RingQueueTest, DequeueFromEmptyQueueReturnsNull) {
    ring_queue<int> q(SMALL_SIZE);
    EXPECT_EQ(q.dequeue(), nullptr);
}

// Single item operations
TEST_F(RingQueueTest, EnqueueAndDequeueOneItem) {
    ring_queue<int> q(SMALL_SIZE);
    q.enqueue(42);

    int* result = q.dequeue();
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(*result, 42);
}

TEST_F(RingQueueTest, DequeueAfterEnqueueReturnsNull) {
    ring_queue<int> q(SMALL_SIZE);
    q.enqueue(42);
    q.dequeue();

    EXPECT_EQ(q.dequeue(), nullptr);
}

// Multiple items
TEST_F(RingQueueTest, EnqueueMultipleItems) {
    ring_queue<int> q(3);
    q.enqueue(1);
    q.enqueue(2);
    q.enqueue(3);

    int* v1 = q.dequeue();
    int* v2 = q.dequeue();
    int* v3 = q.dequeue();

    ASSERT_NE(v1, nullptr);
    ASSERT_NE(v2, nullptr);
    ASSERT_NE(v3, nullptr);

    EXPECT_EQ(*v1, 1);
    EXPECT_EQ(*v2, 2);
    EXPECT_EQ(*v3, 3);
    EXPECT_EQ(q.dequeue(), nullptr);
}

TEST_F(RingQueueTest, ItemsDequeueInFifoOrder) {
    ring_queue<int> q(5);
    std::vector<int> enqueued = {10, 20, 30, 40, 50};

    for (int val : enqueued) {
        q.enqueue(val);
    }

    for (int expected : enqueued) {
        int* result = q.dequeue();
        ASSERT_NE(result, nullptr);
        EXPECT_EQ(*result, expected);
    }
}

// Ring behavior (wraparound)
TEST_F(RingQueueTest, WraparoundBasic) {
    ring_queue<int> q(2);

    // Fill the queue
    q.enqueue(1);
    q.enqueue(2);

    // Dequeue both
    q.dequeue();
    q.dequeue();

    // Enqueue again - this tests the wraparound
    q.enqueue(3);
    q.enqueue(4);

    EXPECT_EQ(*q.dequeue(), 3);
    EXPECT_EQ(*q.dequeue(), 4);
}

TEST_F(RingQueueTest, WraparoundWithInterleavedOps) {
    ring_queue<int> q(4);

    q.enqueue(1);
    q.enqueue(2);
    EXPECT_EQ(*q.dequeue(), 1);

    q.enqueue(3);
    q.enqueue(4);  // producer wraps around

    EXPECT_EQ(*q.dequeue(), 2);
    EXPECT_EQ(*q.dequeue(), 3);
    EXPECT_EQ(*q.dequeue(), 4);
    EXPECT_EQ(q.dequeue(), nullptr);
}

TEST_F(RingQueueTest, CompleteWrapCycle) {
    ring_queue<int> q(2);

    // Cycle 1
    q.enqueue(1);
    q.enqueue(2);
    EXPECT_EQ(*q.dequeue(), 1);
    EXPECT_EQ(*q.dequeue(), 2);

    // Cycle 2 - complete wrap
    q.enqueue(3);
    q.enqueue(4);
    EXPECT_EQ(*q.dequeue(), 3);
    EXPECT_EQ(*q.dequeue(), 4);

    // Cycle 3
    q.enqueue(5);
    q.enqueue(6);
    EXPECT_EQ(*q.dequeue(), 5);
    EXPECT_EQ(*q.dequeue(), 6);
}

// Overwrite behavior (producer catches consumer)
TEST_F(RingQueueTest, ProducerOverwritesWhenBufferFull) {
    ring_queue<int> q(2);

    q.enqueue(1);
    q.enqueue(2);
    q.enqueue(3);  // This overwrites position 0 (where 1 was), advancing consumer

    int* val1 = q.dequeue();
    ASSERT_NE(val1, nullptr);
    // Should get 2 (from position 1) since 1 was overwritten
    EXPECT_EQ(*val1, 2);

    int* val2 = q.dequeue();
    ASSERT_NE(val2, nullptr);
    // Should get 3 (overwrote position 0)
    EXPECT_EQ(*val2, 3);
}

TEST_F(RingQueueTest, MultipleOverwrites) {
    ring_queue<int> q(3);

    // Fill queue
    q.enqueue(1);
    q.enqueue(2);
    q.enqueue(3);

    // Overwrite multiple positions
    q.enqueue(4);  // Overwrites 1
    q.enqueue(5);  // Overwrites 2

    EXPECT_EQ(*q.dequeue(), 3);  // First value still in queue
    EXPECT_EQ(*q.dequeue(), 4);
    EXPECT_EQ(*q.dequeue(), 5);
}

// Different data types
TEST_F(RingQueueTest, WorksWithFloats) {
    ring_queue<float> q(3);

    q.enqueue(3.14f);
    q.enqueue(2.71f);

    float* pi = q.dequeue();
    ASSERT_NE(pi, nullptr);
    EXPECT_FLOAT_EQ(*pi, 3.14f);
}

TEST_F(RingQueueTest, WorksWithStructs) {
    struct Point {
        int x, y;
        bool operator==(const Point& other) const {
            return x == other.x && y == other.y;
        }
    };

    ring_queue<Point> q(2);

    q.enqueue({10, 20});
    q.enqueue({30, 40});

    Point* p1 = q.dequeue();
    ASSERT_NE(p1, nullptr);
    EXPECT_EQ(p1->x, 10);
    EXPECT_EQ(p1->y, 20);

    Point* p2 = q.dequeue();
    ASSERT_NE(p2, nullptr);
    EXPECT_EQ(p2->x, 30);
    EXPECT_EQ(p2->y, 40);
}

// Large buffer tests
TEST_F(RingQueueTest, LargeBufferSequentialAccess) {
    ring_queue<int> q(LARGE_SIZE);

    // Enqueue many items
    for (size_t i = 0; i < MEDIUM_SIZE; i++) {
        q.enqueue(i);
    }

    // Dequeue all and verify order
    for (size_t i = 0; i < MEDIUM_SIZE; i++) {
        int* val = q.dequeue();
        ASSERT_NE(val, nullptr);
        EXPECT_EQ(*val, i);
    }

    EXPECT_EQ(q.dequeue(), nullptr);
}

TEST_F(RingQueueTest, LargeBufferWithManyWraps) {
    ring_queue<int> q(10);

    // Multiple cycles through the buffer
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int i = 0; i < 10; i++) {
            q.enqueue(cycle * 10 + i);
        }

        for (int i = 0; i < 10; i++) {
            int* val = q.dequeue();
            ASSERT_NE(val, nullptr);
            EXPECT_EQ(*val, cycle * 10 + i);
        }
    }
}

// Edge cases
TEST_F(RingQueueTest, QueueOfSizeOne) {
    ring_queue<int> q(1);

    q.enqueue(42);
    EXPECT_EQ(*q.dequeue(), 42);

    q.enqueue(99);
    EXPECT_EQ(*q.dequeue(), 99);
}

TEST_F(RingQueueTest, AlternatingEnqueueDequeue) {
    ring_queue<int> q(2);

    for (int i = 0; i < 5; i++) {
        q.enqueue(i);
        int* val = q.dequeue();
        ASSERT_NE(val, nullptr);
        EXPECT_EQ(*val, i);
    }
}

TEST_F(RingQueueTest, PointerValidityAcrossOperations) {
    ring_queue<int> q(3);

    q.enqueue(1);
    q.enqueue(2);
    q.enqueue(3);

    int* ptr1 = q.dequeue();
    int* ptr2 = q.dequeue();
    int* ptr3 = q.dequeue();

    ASSERT_NE(ptr1, nullptr);
    ASSERT_NE(ptr2, nullptr);
    ASSERT_NE(ptr3, nullptr);

    // Pointers should be different
    EXPECT_NE(ptr1, ptr2);
    EXPECT_NE(ptr2, ptr3);
    EXPECT_NE(ptr1, ptr3);

    // Values should be correct
    EXPECT_EQ(*ptr1, 1);
    EXPECT_EQ(*ptr2, 2);
    EXPECT_EQ(*ptr3, 3);
}

// Stress tests
TEST_F(RingQueueTest, StressSmallBuffer) {
    ring_queue<int> q(5);

    for (int cycle = 0; cycle < 100; cycle++) {
        for (int i = 0; i < 5; i++) {
            q.enqueue(cycle * 5 + i);
        }

        for (int i = 0; i < 5; i++) {
            int* val = q.dequeue();
            ASSERT_NE(val, nullptr);
            EXPECT_EQ(*val, cycle * 5 + i);
        }
    }
}

TEST_F(RingQueueTest, MixedEnqueueDequeue) {
    ring_queue<int> q(15);  // Larger buffer for safety

    int enqueued = 0;
    int dequeued_count = 0;

    // Pattern: enqueue 3, dequeue 2, repeat
    for (int cycle = 0; cycle < 10; cycle++) {
        for (int i = 0; i < 3; i++) {
            q.enqueue(enqueued++);
        }

        for (int i = 0; i < 2; i++) {
            int* val = q.dequeue();
            if (val != nullptr) {
                dequeued_count++;
            }
        }
    }

    // After 10 cycles: enqueued 30, dequeued 20
    // So 10 items should remain
    int* val;
    int remaining = 0;
    while ((val = q.dequeue()) != nullptr) {
        remaining++;
    }

    // Should have 10 items left (30 enqueued - 20 dequeued)
    EXPECT_EQ(remaining, 10);
    // Total dequeued so far
    EXPECT_EQ(dequeued_count, 20);
    // Total should be 30
    EXPECT_EQ(dequeued_count + remaining, 30);
}
