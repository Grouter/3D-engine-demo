#if !defined(HOTLOAD_H)
#define HOTLOAD_H

enum HotloadResource {
    Materials,
    Meshes
};

struct HotloadShaderEntry {
    char shader_name[64];
};

#endif
