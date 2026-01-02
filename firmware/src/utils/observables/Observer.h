#pragma once

namespace utils::observables {
    template <class T>
    class Observer {
    public:
        virtual ~Observer() = default;
        virtual void observe(const T& event) = 0;
    };
}
