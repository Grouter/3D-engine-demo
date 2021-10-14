#if !defined(RESOURCES_H)
#define RESOURCES_H

enum ShaderResource {
    Default
};

struct Resources {
    Mesh     meshes[2];
    Program  programs[1];
    u32      textures[3];
    Material materials[2];
};

#endif
