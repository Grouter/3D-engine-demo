internal Program load_program(const char *name) {
    Program program;

    FILE *shader_file = fopen(name, "r");
    if (!shader_file) {
        printf("Error compiling shader file!\n");
        exit(1);
    }

    char *shader_source;
    u64   shader_file_bytes;

    {   // Allocate enough bytes for reading the shader file
        fseek(shader_file, 0, SEEK_END);
        shader_file_bytes = ftell(shader_file);
        rewind(shader_file);

        shader_source = new char[shader_file_bytes];
    }

    fread(shader_source, sizeof(char), shader_file_bytes, shader_file);
    fclose(shader_file);

    i32  compile_status;
    char compile_log[512];

    u32 vertex_handle;
    u32 fragment_handle;

    {   // Compile the vertex part
        vertex_handle = glCreateShader(GL_VERTEX_SHADER);

        char *vertex_strings[3] = {
            "#version 330\n",
            "#define VERTEX\n",
            shader_source
        };

        glShaderSource(vertex_handle, 3, vertex_strings, NULL);
        glCompileShader(vertex_handle);

        SHADER_COMPILATION_CHECK(vertex_handle, compile_status, compile_log, 512);
    }

    {   // Compile the fragment part
        fragment_handle = glCreateShader(GL_FRAGMENT_SHADER);

        char *fragment_strings[3] = {
            "#version 330\n",
            "#define FRAGMENT\n",
            shader_source
        };

        glShaderSource(fragment_handle, 3, fragment_strings, NULL);
        glCompileShader(fragment_handle);

        SHADER_COMPILATION_CHECK(fragment_handle, compile_status, compile_log, 512);
    }

    program.handle = glCreateProgram();
    glAttachShader(program.handle, vertex_handle);
    glAttachShader(program.handle, fragment_handle);
    glLinkProgram(program.handle);

    delete[] shader_source;

    return program;
}

internal Mesh create_rectangle() {
    Mesh mesh;

    mesh.verticies = new VertexP[] {
        VertexP { make_vector3( 0.5f,  0.5f, 0.0f) },
        VertexP { make_vector3( 0.5f, -0.5f, 0.0f) },
        VertexP { make_vector3(-0.5f, -0.5f, 0.0f) },
        VertexP { make_vector3(-0.5f,  0.5f, 0.0f) },
    };

    mesh.indicies = new u32[] {
        0, 1, 3,
        1, 2, 3
    };

    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    {   // Vertex data
        glGenBuffers(1, &mesh.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
        // @Hardcode
        glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(VertexP), mesh.verticies, GL_STATIC_DRAW);
    }

    {   // Index data
        glGenBuffers(1, &mesh.ebo);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
        // @Hardcode
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(u32), mesh.indicies, GL_STATIC_DRAW);
    }

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    return mesh;
}

internal void bind_mesh(Mesh &mesh) {

}
