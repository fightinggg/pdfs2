#pragma once

template<class T>
 class Supplayer {
public:

    virtual  ~Supplayer() = default;

    virtual T *get();
};

template<class T>
T *Supplayer<T>::get() {
    return nullptr;
}
