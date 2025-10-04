#pragma once

#include <list>

#include "Observer.h"

template <class T>
class Subject {
public:
    Subject() = default;
    ~Subject(){
        this->_observers.clear();
    };

    void addObserver(Observer<T>* observer){
        this->_observers.push_back(observer);
    };
    void removeObserver(Observer<T>* observer){
        this->_observers.remove(observer);
    };

    void notify(const T& value) {
        for (auto observer : this->_observers) {
            observer->observe(value);
        }
    }

private:
    std::list<Observer<T>*> _observers;
};
