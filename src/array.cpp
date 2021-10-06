template <typename T>
internal void allocate_array(Array<T> &array, u64 size) {
    array.length = 0;
    array.capacity = size;

    if (size == 0) {
        array.data = nullptr;
    }
    else {
        array.data = (T*)malloc(sizeof(T) * size);
    }
}

template <typename T>
void Array<T>::reserve(u32 amount) {
    u64 new_capacity = this->capacity + amount;

    if (this->data) {
        this->data = (T*)realloc(this->data, new_capacity * sizeof(T));
    }
    else {
        this->data = (T*)malloc(new_capacity * sizeof(T));
        this->length = 0;   // Just to be sure...
    }

    this->capacity = new_capacity;
}

template <typename T>
void Array<T>::add(T item) {
    if (this->length >= this->capacity) {
        // @Todo: do we want to +1 the capacity or more?
        this->reserve(1);
    }

    assert(this->length < this->capacity);

    this->data[this->length] = item;
    this->length += 1;
}

template <typename T>
void Array<T>::remove(u64 index) {
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

template <typename T>
T* Array<T>::get(u64 index) {
    assert(index < this->length);

    return (this->data + index);
}

template <typename T>
void Array<T>::fast_remove(u64 index) {
    assert(index < this->length);

    if (this->length == 1) {
        this->length = 0;
    }
    // When removing the last element
    else if (index == this->length - 1) {
        this->length -= 1;
    }
    // Move last element to the place of removal.
    // This ensusres that we do not need to shift the rest of the array.
    else {
        this->data[index] = this->data[this->length - 1];

        this->length -= 1;
    }
}
