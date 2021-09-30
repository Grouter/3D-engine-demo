#if !defined(ARRAY_H)
#define ARRAY_H

#define free_array(a) {if ((a).data) { free((a).data); } }

template <typename T>
struct Array {
    u64 length;
    u64 capacity;
    T *data;

    void add(T item);
    void remove(u32 index);
    // @Todo: performant remove (to prevent memmoves)
};

#endif
