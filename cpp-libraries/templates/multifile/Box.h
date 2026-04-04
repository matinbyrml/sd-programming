#ifndef BOX_H
#define BOX_H

template <typename T>
class Box{
    private:
        T item;
    public:
    Box(T initialItem);
    T getItem() const;
    void setItem(T newItem);
};

#include "Box.hpp"
#endif