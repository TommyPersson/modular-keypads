#pragma once

template<class T>
class Observer {
public:
    virtual ~Observer() {}
    virtual void observe(const T& value) {
    };
};
