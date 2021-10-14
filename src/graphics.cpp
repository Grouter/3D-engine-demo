internal Program load_program(const char *name) {
    Program program;

    FILE *shader_file;

    errno_t open_err = fopen_s(&shader_file, name, "r");

    if(open_err) {
        printf("Error opening a shader file: %s\n", name);
        exit(1);
    }
    else {
        printf("Loaded shader: %s\n", name);
    }

    char *shader_source;
    u64   shader_file_bytes;

    {   // Allocate enough bytes for reading the shader file
        fseek(shader_file, 0, SEEK_END);
        shader_file_bytes = ftell(shader_file);
        rewind(shader_file);

        shader_source = (char *)calloc(shader_file_bytes, sizeof(char));
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

        SHADER_COMPILATION_CHECK(vertex_handle, compile_status, compile_log, 512, "Vertex");
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

        SHADER_COMPILATION_CHECK(fragment_handle, compile_status, compile_log, 512, "Fragment");
    }

    program.handle = glCreateProgram();
    glAttachShader(program.handle, vertex_handle);
    glAttachShader(program.handle, fragment_handle);

    glLinkProgram(program.handle);

    glDetachShader(program.handle, vertex_handle);
    glDetachShader(program.handle, fragment_handle);

    free(shader_source);

    return program;
}

internal void bind_mesh_buffer_objects(Mesh &mesh) {
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    {   // Vertex data
        glGenBuffers(1, &mesh.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
        glBufferData(GL_ARRAY_BUFFER, mesh.verticies.length * sizeof(Vector3), mesh.verticies.data, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);
    }

    {   // Normal data
        glGenBuffers(1, &mesh.nbo);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.nbo);
        glBufferData(GL_ARRAY_BUFFER, mesh.normals.length * sizeof(Vector3), mesh.normals.data, GL_STATIC_DRAW);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(1);
    }

    {   // UV data
        glGenBuffers(1, &mesh.tbo);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.tbo);
        glBufferData(GL_ARRAY_BUFFER, mesh.uvs.length * sizeof(Vector2), mesh.uvs.data, GL_STATIC_DRAW);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(2);
    }

    {   // Index data
        glGenBuffers(1, &mesh.ebo);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indicies.length * sizeof(u32), mesh.indicies.data, GL_STATIC_DRAW);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}