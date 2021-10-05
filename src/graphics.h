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

struct VertexP {
    Vector3 position;
};

struct Mesh {
    u32 vao;
    u32 vbo;
    u32 ebo;

    // @Robustness: this can only hold one triangle list
    u32 vertex_count;
    u32 index_count;

    // @Todo: allocate only one buffer for these:
    VertexP *verticies;
    u32     *indicies;
};

#endif
