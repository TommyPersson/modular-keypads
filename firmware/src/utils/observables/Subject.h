#pragma once

#include "Observable.h"

template <class T>
class Subject : public Observable<T> {
public:
    Subject() = default;
    ~Subject() = default;

    void notify(const T& event) {
        for (auto observer : this->_observers) {
            observer->observe(event);
        }
    }
};
