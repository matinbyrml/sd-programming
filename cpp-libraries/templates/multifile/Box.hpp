#ifndef BOX_HPP
#define BOX_HPP
template <typename T>
Box<T>::Box(T initialItem): item(initialItem) {

}

template <typename T>
T Box<T>::getItem() const {
    return item;
}

// Setter Implementation
template <typename T>
void Box<T>::setItem(T newItem) {
    item = newItem;
}

#endif