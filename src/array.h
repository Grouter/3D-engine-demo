#if !defined(ARRAY_H)
#define ARRAY_H

#define free_array(a) {if ((a).data) { free((a).data); } }

template <typename T>
struct Array {
    u64 length;
    u64 capacity;
    T *data;

    void reserve(u32 amount);
    void add(T item);
    void remove(u32 index);

    // This will reorder the array!
    void fast_remove(u32 index);
};

#endif
