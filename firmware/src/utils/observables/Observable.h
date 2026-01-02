#pragma once

#include <list>

#include "Observer.h"

namespace utils::observables {
    template <class T>
    class Observable {
    public:
        Observable() = default;
        ~Observable(){
            this->_observers.clear();
        };

        void addObserver(Observer<T>* observer){
            this->_observers.push_back(observer);
        };
        void removeObserver(Observer<T>* observer){
            this->_observers.remove(observer);
        };

    protected:
        std::list<Observer<T>*> _observers;
    };
}