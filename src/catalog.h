#if !defined(CATALOG_H)
#define CATALOG_H

struct ResourceCatalog {
    u64 size;

    struct Entry {
        char  *key;
        u64   index;
        Entry *next_in_hash;
    };

    Entry **entries;
};

#endif
