#if !defined(HOTLOAD_H)
#define HOTLOAD_H

enum HotloadFileType {
    Shaders,
};

struct HotloadShaderEntry {
    char shader_name[64];
};

#endif
