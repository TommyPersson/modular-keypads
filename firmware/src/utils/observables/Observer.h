#pragma once

template<class T>
class Observer {
public:
    virtual ~Observer() = default;
    virtual void observe(const T& event) = 0;
};
