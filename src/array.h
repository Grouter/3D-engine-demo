#if !defined(ARRAY_H)
#define ARRAY_H

// @Todo: fast add without resize checks

#define free_array(a) {if ((a).data) { free((a).data); } }

#define array_foreach(a, it) for ((it) = (a).data; ((it) - (a).data) < (i64)(a).length; (it)++)

template <typename T>
struct Array {
    u64 length;
    u64 capacity;
    T *data;

    void reserve(u32 amount);
    void add(T item);
    // Adds an item without checking for a resize.
    void fast_add(T item);
    T* get(u64 index);
    void remove(u64 index);
    // This will reorder the array!
    void fast_remove(u64 index);
    void clear();
};

#endif
