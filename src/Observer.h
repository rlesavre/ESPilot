#ifndef OBSERVER_H
#define OBSERVER_H

#include <list>

template <typename T> class Observer {
 public:
  virtual ~Observer(){};
  virtual void notify(T arg) = 0;
};

template <typename T>  class Subject {
 public:
  void registerObserver(Observer<T>* observer) {
    observers.push_back(observer);
  }
  void unregisterObserver(Observer<T>* observer) {
    observers.remove(observer);
  }
  void notifyObservers(T argument) {
    for (auto observer: observers)
    {
      observer->notify(argument);
    }
  }
 private:
  std::list<Observer<T> *> observers;

  //array<Observer<T>*> observers;
};
#endif