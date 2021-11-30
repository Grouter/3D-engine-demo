#if !defined(BUCKET_ARRAY_H)
#define BUCKET_ARRAY_H

// @Todo: refactor all iterations to this macro
//
// While using this iterator macro, put only closing bracket
// Like:
//
// bucket_array_foreach(a, it)
//     code...
// }
//
// or
//
// bucket_array_foreach(a, it) {
//     code...
// }}
//
#define bucket_array_foreach(a, it)\
    for (u32 _bi = 0; _bi < (a).buckets.length; _bi++) for (u32 _si = 0; _si < (a).bucket_size; _si++) {\
        if (!(a).buckets[_bi].occupied[_si]) continue;\
        it = &(a).buckets[_bi].data[_si];

#define bucket_array_foreach_w_loc(a, it, loc)\
    for ((loc).bucket_index = 0; (loc).bucket_index < (a).buckets.length; (loc).bucket_index++)\
        for ((loc).slot_index = 0; (loc).slot_index < (a).bucket_size; (loc).slot_index++) {\
            if (!(a).buckets[(loc).bucket_index].occupied[(loc).slot_index]) continue;\
            it = &(a)[(loc)];

struct BucketLocation {
    u32 bucket_index;
    u32 slot_index;
};

template <typename T>
struct Bucket {
    u32 capacity;
    u32 stored;

    // @Todo: these two could be in one buffer
    // which would require only one allocation
    T    *data;
    bool *occupied;
};

template <typename T>
struct BucketArray {
    u32 stored;
    u32 bucket_size;

    Array<Bucket<T>> buckets;
    Array<u32>       unfull_buckets;

    void add_bucket();
    BucketLocation get_new(T **stored_ptr);
    inline BucketLocation add(T item, T **stored_ptr);
    inline BucketLocation add(T item);
    void remove(BucketLocation location);
    T & operator[] (BucketLocation location);
    void clear();
};

template <typename T>
inline void allocate_bucket_array(BucketArray<T> &array, u32 items_per_bucket) {
    assert(items_per_bucket != 0);

    array.stored = 0;
    array.bucket_size = items_per_bucket;

    allocate_array(array.buckets,        5);
    allocate_array(array.unfull_buckets, 5);
}

template <typename T>
void BucketArray<T>::add_bucket() {
    Bucket<T> new_bucket = {};

    new_bucket.capacity = this->bucket_size;
    new_bucket.stored = 0;

    new_bucket.data     = (T*)calloc(new_bucket.capacity, sizeof(T));
    new_bucket.occupied = (bool*)calloc(new_bucket.capacity, sizeof(bool));

    this->buckets.add(new_bucket);
    this->unfull_buckets.add((u32)this->buckets.length - 1);
}

template <typename T>
BucketLocation BucketArray<T>::get_new(T **stored_ptr) {
    *stored_ptr = nullptr;   // By default return null

    if (this->unfull_buckets.length == 0) {
        this->add_bucket();
    }

    assert(this->unfull_buckets.length != 0);

    // We always pick the first in the unfull bucket list
    u32 unfull_bucket_index = this->unfull_buckets[0];
    Bucket<T> *target = &this->buckets[unfull_bucket_index];

    assert(target->stored < target->capacity);

    BucketLocation result = {};
    result.bucket_index = unfull_bucket_index;

    // Find and occupy empty slot
    {
        bool store_success = false;

        // @Speed: maybe we can cache this
        for (u32 i = 0; i < target->capacity; i++) {
            if (target->occupied[i]) continue;

            target->occupied[i] = true;
            target->stored += 1;
            this->stored += 1;

            // Fill the return ptr
            *stored_ptr = &target->data[i];

            result.slot_index = i;

            store_success = true;

            break;
        }

        // Adding to unfull bucket which is apparently full!
        assert(store_success);
    }

    if (target->stored >= target->capacity) {
        this->unfull_buckets.fast_remove(0);

        if (this->unfull_buckets.length > 1) {
            std::sort(this->unfull_buckets.data, this->unfull_buckets.data + this->unfull_buckets.length);
        }
    }

    return result;
}

template <typename T>
inline BucketLocation BucketArray<T>::add(T item, T **stored_ptr) {
    BucketLocation result = get_new(stored_ptr);

    **stored_ptr = item;

    return result;
}

template <typename T>
inline BucketLocation BucketArray<T>::add(T item) {
    T *ptr;
    BucketLocation result = get_new(&ptr);

    *ptr = item;

    return result;
}

template <typename T>
void BucketArray<T>::remove(BucketLocation location) {
    assert(location.bucket_index < this->buckets.length);
    assert(location.slot_index < this->bucket_size);

    Bucket<T> *target = &this->buckets[location.bucket_index];

    // Removing slot which is not occupied!
    assert(target->occupied[location.slot_index] == true);

    target->occupied[location.slot_index] = false;
    target->stored -= 1;
    this->stored -= 1;

    bool is_in_unfull = false;

    for (u64 i = 0; i < this->unfull_buckets.length; i++) {
        if (this->unfull_buckets[i] == location.bucket_index) {
            is_in_unfull = true;
            break;
        }
    }

    if (!is_in_unfull) {
        this->unfull_buckets.add(location.bucket_index);

        if (this->unfull_buckets.length > 1) {
            std::sort(this->unfull_buckets.data, this->unfull_buckets.data + this->unfull_buckets.length);
        }
    }
}

template <typename T>
inline T& BucketArray<T>::operator[] (BucketLocation location) {
    assert(location.bucket_index < this->buckets.length);
    assert(location.slot_index < this->bucket_size);

    // To avoid silent errors...
    // assert(this->buckets[location.bucket_index].occupied[location.slot_index] == true);

    T &result = this->buckets[location.bucket_index].data[location.slot_index];

    return result;
}

template <typename T>
void BucketArray<T>::clear() {
    this->stored = 0;
    this->buckets.clear();
    this->unfull_buckets.clear();
}

#endif