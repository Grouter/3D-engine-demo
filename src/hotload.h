#if !defined(HOTLOAD_H)
#define HOTLOAD_H

enum HotloadResource {
    HotloadResource_MATERIALS,
    HotloadResource_MESHES,
    HotloadResource_WORLD,

    HotloadResource_COUNT
};

struct HotloadShaderEntry {
    char shader_name[64];
};

#endif
