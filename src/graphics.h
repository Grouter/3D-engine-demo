#if !defined(GRAPHICS_H)
#define GRAPHICS_H

#define SHADER_COMPILATION_CHECK(handle,status,log,log_size,log_type) {\
    glGetShaderiv(handle, GL_COMPILE_STATUS, &status);\
    if (!status) {\
        glGetShaderInfoLog(handle, log_size, NULL, log);\
        log_print("%s shader compilation failed:\n\n", log_type);\
        log_print("%s\n", log);\
    }\
}

struct Program {
    u32 handle;

    // Used for lookups
    bool has_geometry = false;
    Array<char *> defines = {};
    u64 source_file_hash;
};

struct Material {
    u32 texture;
    Vector4 color = V4_ONE;
};

struct SubMeshInfo {
    u64 material_index;
    u32 first_index;
    u32 index_count;
};

struct Mesh {
    bool32 loaded;

    // OpenGL buffers
    u32 vao;
    u32 vbo;
    u32 nbo;
    u32 ebo;
    u32 tbo;

    // Loaded data
    Array<SubMeshInfo> sub_meshes;
    Array<Vector3>     verticies;
    Array<Vector3>     normals;
    Array<u32>         indicies;
    Array<Vector2>     uvs;
};

#endif
