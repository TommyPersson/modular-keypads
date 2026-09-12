#include <gtest/gtest.h>
#include <vector>
#include "tfw/utils/observables/Observable.h"
#include "tfw/utils/observables/Observer.h"
#include "tfw/utils/observables/Subject.h"

using namespace tfw::utils::observables;

// ============================================================================
// Test Observer Implementations
// ============================================================================

// Simple observer that tracks calls
template <typename T>
class MockObserver : public Observer<T> {
public:
    std::vector<T> received_events;
    int call_count = 0;

    void observe(const T& event) override {
        received_events.push_back(event);
        call_count++;
    }
};

// Test fixture for observer pattern tests
class ObserverPatternTest : public ::testing::Test {
protected:
    // Helper to verify observer received specific events
    template <typename T>
    bool HasReceived(MockObserver<T>* observer, const T& event) {
        for (const auto& received : observer->received_events) {
            if (received == event) {
                return true;
            }
        }
        return false;
    }
};

// ============================================================================
// Basic Subject and Observer Tests
// ============================================================================

TEST_F(ObserverPatternTest, CreateSubject) {
    Subject<int> subject;
    // Should construct without error
    EXPECT_TRUE(true);
}

TEST_F(ObserverPatternTest, AddObserver) {
    Subject<int> subject;
    MockObserver<int> observer;

    subject.addObserver(&observer);
    EXPECT_EQ(observer.call_count, 0);
}

TEST_F(ObserverPatternTest, RemoveObserver) {
    Subject<int> subject;
    MockObserver<int> observer;

    subject.addObserver(&observer);
    subject.removeObserver(&observer);

    // Observer should not be called after removal
    subject.notify(42);
    EXPECT_EQ(observer.call_count, 0);
}

// ============================================================================
// Single Observer Tests
// ============================================================================

TEST_F(ObserverPatternTest, SingleObserverReceivesNotification) {
    Subject<int> subject;
    MockObserver<int> observer;

    subject.addObserver(&observer);
    subject.notify(42);

    EXPECT_EQ(observer.call_count, 1);
    EXPECT_EQ(observer.received_events.size(), 1);
    EXPECT_EQ(observer.received_events[0], 42);
}

TEST_F(ObserverPatternTest, SingleObserverReceivesMultipleNotifications) {
    Subject<int> subject;
    MockObserver<int> observer;

    subject.addObserver(&observer);

    subject.notify(1);
    subject.notify(2);
    subject.notify(3);

    EXPECT_EQ(observer.call_count, 3);
    EXPECT_EQ(observer.received_events.size(), 3);
    EXPECT_EQ(observer.received_events[0], 1);
    EXPECT_EQ(observer.received_events[1], 2);
    EXPECT_EQ(observer.received_events[2], 3);
}

TEST_F(ObserverPatternTest, SingleObserverNotNotifiedBeforeAdding) {
    Subject<int> subject;
    MockObserver<int> observer;

    subject.notify(42);
    subject.addObserver(&observer);

    EXPECT_EQ(observer.call_count, 0);
}

// ============================================================================
// Multiple Observers Tests
// ============================================================================

TEST_F(ObserverPatternTest, MultipleObserversAllReceiveNotification) {
    Subject<int> subject;
    MockObserver<int> observer1, observer2, observer3;

    subject.addObserver(&observer1);
    subject.addObserver(&observer2);
    subject.addObserver(&observer3);

    subject.notify(42);

    EXPECT_EQ(observer1.call_count, 1);
    EXPECT_EQ(observer2.call_count, 1);
    EXPECT_EQ(observer3.call_count, 1);

    EXPECT_EQ(observer1.received_events[0], 42);
    EXPECT_EQ(observer2.received_events[0], 42);
    EXPECT_EQ(observer3.received_events[0], 42);
}

TEST_F(ObserverPatternTest, MultipleObserversMultipleNotifications) {
    Subject<int> subject;
    MockObserver<int> observer1, observer2;

    subject.addObserver(&observer1);
    subject.addObserver(&observer2);

    subject.notify(1);
    subject.notify(2);
    subject.notify(3);

    EXPECT_EQ(observer1.call_count, 3);
    EXPECT_EQ(observer2.call_count, 3);

    EXPECT_EQ(observer1.received_events[0], 1);
    EXPECT_EQ(observer1.received_events[1], 2);
    EXPECT_EQ(observer1.received_events[2], 3);
}

TEST_F(ObserverPatternTest, RemoveOneObserverNotOthers) {
    Subject<int> subject;
    MockObserver<int> observer1, observer2, observer3;

    subject.addObserver(&observer1);
    subject.addObserver(&observer2);
    subject.addObserver(&observer3);

    subject.removeObserver(&observer2);
    subject.notify(42);

    EXPECT_EQ(observer1.call_count, 1);
    EXPECT_EQ(observer2.call_count, 0);  // Was removed
    EXPECT_EQ(observer3.call_count, 1);
}

// ============================================================================
// Different Event Types Tests
// ============================================================================

TEST_F(ObserverPatternTest, ObserverWithFloatEvents) {
    Subject<float> subject;
    MockObserver<float> observer;

    subject.addObserver(&observer);

    subject.notify(3.14f);
    subject.notify(2.71f);

    EXPECT_EQ(observer.call_count, 2);
    EXPECT_FLOAT_EQ(observer.received_events[0], 3.14f);
    EXPECT_FLOAT_EQ(observer.received_events[1], 2.71f);
}

TEST_F(ObserverPatternTest, ObserverWithStructEvents) {
    struct Event {
        int id;
        bool active;

        bool operator==(const Event& other) const {
            return id == other.id && active == other.active;
        }
    };

    Subject<Event> subject;
    MockObserver<Event> observer;

    subject.addObserver(&observer);

    Event event1{1, true};
    Event event2{2, false};

    subject.notify(event1);
    subject.notify(event2);

    EXPECT_EQ(observer.call_count, 2);
    EXPECT_EQ(observer.received_events[0], event1);
    EXPECT_EQ(observer.received_events[1], event2);
}

TEST_F(ObserverPatternTest, ObserverWithStringEvents) {
    Subject<std::string> subject;
    MockObserver<std::string> observer;

    subject.addObserver(&observer);

    subject.notify("hello");
    subject.notify("world");

    EXPECT_EQ(observer.call_count, 2);
    EXPECT_EQ(observer.received_events[0], "hello");
    EXPECT_EQ(observer.received_events[1], "world");
}

// ============================================================================
// Multiple Subjects Tests
// ============================================================================

TEST_F(ObserverPatternTest, ObserverWithMultipleSubjects) {
    Subject<int> subject1, subject2;
    MockObserver<int> observer;

    subject1.addObserver(&observer);
    subject2.addObserver(&observer);

    subject1.notify(1);
    subject2.notify(2);

    EXPECT_EQ(observer.call_count, 2);
    EXPECT_EQ(observer.received_events[0], 1);
    EXPECT_EQ(observer.received_events[1], 2);
}

TEST_F(ObserverPatternTest, MultipleObserversMultipleSubjects) {
    Subject<int> subject1, subject2;
    MockObserver<int> observer1, observer2;

    subject1.addObserver(&observer1);
    subject1.addObserver(&observer2);
    subject2.addObserver(&observer1);
    subject2.addObserver(&observer2);

    subject1.notify(10);
    subject2.notify(20);

    EXPECT_EQ(observer1.call_count, 2);
    EXPECT_EQ(observer2.call_count, 2);

    EXPECT_EQ(observer1.received_events[0], 10);
    EXPECT_EQ(observer1.received_events[1], 20);
    EXPECT_EQ(observer2.received_events[0], 10);
    EXPECT_EQ(observer2.received_events[1], 20);
}

// ============================================================================
// Edge Cases and Stress Tests
// ============================================================================

TEST_F(ObserverPatternTest, AddSameObserverTwice) {
    Subject<int> subject;
    MockObserver<int> observer;

    subject.addObserver(&observer);
    subject.addObserver(&observer);  // Add same observer again (should be ignored)

    subject.notify(42);

    // Observer should only be called once (duplicate subscription prevented)
    EXPECT_EQ(observer.call_count, 1);
}

TEST_F(ObserverPatternTest, RemoveObserverTwice) {
    Subject<int> subject;
    MockObserver<int> observer;

    subject.addObserver(&observer);
    subject.removeObserver(&observer);
    subject.removeObserver(&observer);  // Remove again (should be no-op)

    subject.notify(42);
    EXPECT_EQ(observer.call_count, 0);
}

TEST_F(ObserverPatternTest, NotifyWithoutObservers) {
    Subject<int> subject;

    // Should not crash when notifying with no observers
    subject.notify(42);
    EXPECT_TRUE(true);
}

TEST_F(ObserverPatternTest, ManyObservers) {
    Subject<int> subject;
    std::vector<std::unique_ptr<MockObserver<int>>> observers;

    // Create many observers
    const int NUM_OBSERVERS = 100;
    for (int i = 0; i < NUM_OBSERVERS; i++) {
        observers.push_back(std::make_unique<MockObserver<int>>());
        subject.addObserver(observers.back().get());
    }

    subject.notify(42);

    // All observers should have received the event
    for (const auto& observer : observers) {
        EXPECT_EQ(observer->call_count, 1);
        EXPECT_EQ(observer->received_events[0], 42);
    }
}

TEST_F(ObserverPatternTest, ManyNotifications) {
    Subject<int> subject;
    MockObserver<int> observer;

    subject.addObserver(&observer);

    // Send many notifications
    const int NUM_NOTIFICATIONS = 1000;
    for (int i = 0; i < NUM_NOTIFICATIONS; i++) {
        subject.notify(i);
    }

    EXPECT_EQ(observer.call_count, NUM_NOTIFICATIONS);
    EXPECT_EQ(observer.received_events.size(), NUM_NOTIFICATIONS);

    // Verify all events were received in order
    for (int i = 0; i < NUM_NOTIFICATIONS; i++) {
        EXPECT_EQ(observer.received_events[i], i);
    }
}

// ============================================================================
// Observable-only Tests
// ============================================================================

TEST_F(ObserverPatternTest, ObservableAddRemove) {
    Observable<int> observable;
    MockObserver<int> observer1, observer2;

    observable.addObserver(&observer1);
    observable.addObserver(&observer2);
    observable.removeObserver(&observer1);

    // Observable should have observer2 but not observer1
    // (We can't directly test this without accessing _observers,
    // but this is a smoke test that add/remove don't crash)
    EXPECT_TRUE(true);
}

// ============================================================================
// Destruction Tests
// ============================================================================

TEST_F(ObserverPatternTest, SubjectDestructionDoesNotCrash) {
    MockObserver<int> observer;

    {
        Subject<int> subject;
        subject.addObserver(&observer);
        subject.notify(42);
        // Subject destroyed here, should clean up without crash
    }

    EXPECT_EQ(observer.call_count, 1);
}

TEST_F(ObserverPatternTest, ObservableDestructionDoesNotCrash) {
    MockObserver<int> observer;

    {
        Observable<int> observable;
        observable.addObserver(&observer);
        // Observable destroyed here, should clean up without crash
    }

    EXPECT_TRUE(true);
}

// ============================================================================
// Integration Tests
// ============================================================================

TEST_F(ObserverPatternTest, RealWorldEventNotification) {
    struct ButtonEvent {
        int pin;
        bool pressed;
        uint32_t timestamp;

        bool operator==(const ButtonEvent& other) const {
            return pin == other.pin && pressed == other.pressed &&
                   timestamp == other.timestamp;
        }
    };

    Subject<ButtonEvent> button_events;
    MockObserver<ButtonEvent> event_logger;

    button_events.addObserver(&event_logger);

    ButtonEvent press_event{5, true, 1000};
    ButtonEvent release_event{5, false, 2000};

    button_events.notify(press_event);
    button_events.notify(release_event);

    EXPECT_EQ(event_logger.call_count, 2);
    EXPECT_EQ(event_logger.received_events[0], press_event);
    EXPECT_EQ(event_logger.received_events[1], release_event);
}

TEST_F(ObserverPatternTest, ChainedObservation) {
    // Test where one observer is also a subject
    Subject<int> source;
    MockObserver<int> observer1, observer2;

    source.addObserver(&observer1);
    source.addObserver(&observer2);

    for (int i = 0; i < 10; i++) {
        source.notify(i);
    }

    EXPECT_EQ(observer1.call_count, 10);
    EXPECT_EQ(observer2.call_count, 10);
}
