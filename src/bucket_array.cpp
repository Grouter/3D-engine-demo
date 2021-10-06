template <typename T>
inline void allocate_bucket_array(BucketArray<T> &array, u32 items_per_bucket) {
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
    this->unfull_buckets.add(this->buckets.length - 1);
}

template <typename T>
void BucketArray<T>::add(T item) {
    if (this->unfull_buckets.length == 0) {
        this->add_bucket();
    }

    assert(this->unfull_buckets.length != 0);

    // We always pick the first in the unfull bucket list
    u64 unfull_bucket_index = this->unfull_buckets.data[0];
    Bucket<T> *target = (this->buckets.data + unfull_bucket_index);

    assert(target->stored < target->capacity);

    // Find and occupy empty slot
    {
        bool store_success = false;

        // @Speed: maybe we can cache this
        for (u64 i = 0; i < target->capacity; i++) {
            if (target->occupied[i]) continue;

            target->data[i]     = item;
            target->occupied[i] = true;
            target->stored += 1;

            store_success = true;

            break;
        }

        assert(store_success);
    }

    if (target->stored >= target->capacity) {
        this->unfull_buckets.fast_remove(0);

        if (this->unfull_buckets.length > 1) {
            std::sort(this->unfull_buckets.data, this->unfull_buckets.data + this->unfull_buckets.length);
        }
    }
}

template <typename T>
void BucketArray<T>::remove(BucketLocation location) {
    assert(location.bucket_index < this->buckets.length);
    assert(location.slot_index < this->bucket_size);

    Bucket<T> *target = this->buckets.get(location.bucket_index);

    assert(target->occupied[location.slot_index] == true);

    target->occupied[location.slot_index] = false;
    target->stored -= 1;

    bool is_in_unfull = false;

    for (u64 i = 0; i < this->unfull_buckets.length; i++) {
        if (this->unfull_buckets.data[i] == location.bucket_index) {
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
T* BucketArray<T>::get(BucketLocation location) {
    assert(location.bucket_index < this->buckets.length);
    assert(location.slot_index < this->bucket_size);

    Bucket<T> *target = this->buckets.data + location.bucket_index;

    // To avoid silent errors...
    assert(target->occupied[location.slot_index] == true);

    T *result = (target->data + location.slot_index);

    return result;
}