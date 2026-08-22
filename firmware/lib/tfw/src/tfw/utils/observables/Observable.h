#pragma once

#include <algorithm>
#include <list>
#include <ranges>

#include "Observer.h"

namespace tfw::utils::observables {
    template <class T>
    class Observable {
    public:
        Observable() = default;

        ~Observable() {
            this->_observers.clear();
        };

        void addObserver(Observer<T>* observer) {
            auto existing = std::find(this->_observers.begin(), this->_observers.end(), observer);
            if (existing == this->_observers.end()) {
                this->_observers.push_back(observer);
            }
        };

        void removeObserver(Observer<T>* observer) {
            this->_observers.remove(observer);
        };

    protected:
        std::list<Observer<T>*> _observers;
    };
}
