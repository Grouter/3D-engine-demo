#if !defined(GRAPHICS_H)
#define GRAPHICS_H

#define SHADER_COMPILATION_CHECK(handle,status,log,log_size,log_type) {\
    glGetShaderiv(handle, GL_COMPILE_STATUS, &status);\
    if (!status) {\
        glGetShaderInfoLog(handle, log_size, NULL, log);\
        printf("%s shader compilation failed:\n%s\n", log_type, log);\
        exit(1);\
    }\
}

struct Program {
    u32 handle;
};

struct SubMeshInfo {
    // @Todo: submesh material index
    u32 fist_index;
    u32 index_count;
};

struct Mesh {
    u32 vao;
    u32 vbo;
    u32 nbo;
    u32 ebo;

    Array<SubMeshInfo> sub_meshes;

    // Loaded data
    Array<Vector3> verticies;
    Array<Vector3> normals;
    Array<u32>     indicies;
};

#endif
