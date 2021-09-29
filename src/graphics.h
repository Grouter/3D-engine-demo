#if !defined(GRAPHICS_H)
#define GRAPHICS_H

#define SHADER_COMPILATION_CHECK(handle,status,log,log_size) {\
    glGetShaderiv(handle, GL_COMPILE_STATUS, &status);\
    if (!status) {\
        glGetShaderInfoLog(handle, log_size, NULL, log);\
        printf("Shader compilation failed:\n%s\n", log);\
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

    VertexP *verticies;
    u32     *indicies;
};

#endif
