#if !defined(RESOURCES_H)
#define RESOURCES_H

struct ResourceCatalog {
    u64 size;

    struct Entry {
        char  *key;
        void  *data;
        Entry *next_in_hash;
    };

    Entry **entries;
};

enum ShaderResource {
    Default
};

struct Resources {
    ResourceCatalog mesh_catalog;
    ResourceCatalog texture_catalog;
    ResourceCatalog material_catalog;

    Mesh     meshes[2];
    Program  programs[1];
    u32      textures[3];
    Material materials[2];
};

#endif
