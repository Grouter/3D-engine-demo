#if !defined(BUCKET_ARRAY_H)
#define BUCKET_ARRAY_H

struct BucketLocation {
    u64 bucket_index;
    u64 slot_index;
};

template <typename T>
struct Bucket {
    u64 capacity;
    u64 stored;

    T    *data;
    bool *occupied;
};

template <typename T>
struct BucketArray {
    u64 stored;
    u64 bucket_size;

    Array<Bucket<T>> buckets;
    Array<u64>       unfull_buckets;

    void add_bucket();
    void add(T item);
    void remove(BucketLocation location);
    T *get(BucketLocation location);
};

#endif
