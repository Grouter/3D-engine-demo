#if !defined(RESOURCES_H)
#define RESOURCES_H

enum ProgramResource {
    Default
};

struct Resources {
    Mesh meshes[2];
    Program programs[1];
    u32 textures[2];
};

#endif
