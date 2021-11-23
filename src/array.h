#if !defined(ARRAY_H)
#define ARRAY_H

#define free_array(a) {if ((a).data) { free((a).data); } }

#define array_foreach(a, it) for ((it) = (a).data; ((it) - (a).data) < (i64)(a).length; (it)++)
#define array_iterate_back(a, it, idx) for ((idx) = ((a).length - 1), (it) = ((a).data + (a).length - 1); (idx) >= 0; (idx)--, (it)--)

template <typename T>
struct Array {
    u64 length;
    u64 capacity;
    T *data;

    inline bool is_full();

    void reserve(u32 amount);

    void add(T item);
    // Adds an item without checking for a resize.
    void fast_add(T item);
    // Allocates space from item and returns pointer to that space.
    T* allocate();

    T& operator[] (u64 index);
    T& last();

    void remove(u64 index);
    // This will reorder the array!
    void fast_remove(u64 index);
    void remove_last();
    void remove_last_and_zero_out();

    void clear();
};

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
inline bool Array<T>::is_full() {
    return this->length == this->capacity;
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
T* Array<T>::allocate() {
    if (this->length >= this->capacity) {
        // @Todo: do we want to +1 the capacity or more?
        this->reserve(1);
    }

    assert(this->length < this->capacity);

    this->length += 1;

    return (this->data + this->length - 1);
}

template <typename T>
void Array<T>::fast_add(T item) {
    assert(this->length < this->capacity);

    this->data[this->length] = item;
    this->length += 1;
}

template <typename T>
T& Array<T>::operator[] (u64 index) {
    assert(index < this->length);

    T& result = this->data[index];

    return result;
}

template <typename T>
T& Array<T>::last() {
    assert(this->length > 0);

    return (this->data + this->length - 1);
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

template <typename T>
void Array<T>::remove_last() {
    assert(this->length > 0);

    this->length--;
}

template <typename T>
void Array<T>::remove_last_and_zero_out() {
    assert(this->length > 0);

    memset(this->data + this->length - 1, 0, sizeof(T));

    this->length--;
}

template <typename T>
void Array<T>::clear() {
    this->length = 0;
}

#endif