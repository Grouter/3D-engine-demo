#include "platform.h"
#include "array.h"

template <typename T>
internal Array<T> allocate_array(u64 size) {
    Array<T> array;

    array.length = 0;
    array.capacity = size;

    if (size == 0) {
        array.data = nullptr;
    }
    else {
        array.data = (T*)malloc(sizeof(T) * size);
    }

    return array;
}

template <typename T>
void Array<T>::add(T item) {
    if (this->length >= this->capacity) {
        // @Todo: do we want to +1 the capacity or more?
        u64 new_capacity = this->capacity + 1;

        if (this->data) {
            this->data = (T*)realloc(this->data, new_capacity * sizeof(item));
        }
        else {
            this->data = (T*)malloc(new_capacity * sizeof(item));
            this->length = 0;   // Just to be sure...
        }

        this->capacity = new_capacity;
    }

    assert(this->length <= this->capacity);

    this->data[this->length] = item;
    this->length += 1;
}

template <typename T>
void Array<T>::remove(u32 index) {
    assert(index < this->length);

    // When removing the last element
    if (index == this->length - 1) {
        this->length -= 1;
    }
    else {
        u64 right_part = this->length - 1 - index;
        memmove(this->data + index, this->data + index + 1, sizeof(T) * right_part);
        this->length -= 1;
    }
}
