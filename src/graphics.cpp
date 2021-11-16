internal bool load_program(const char *path, Program &shader) {
    Array<char> shader_source;

    bool success = read_whole_file(path, shader_source);

    if (!success) {
        return false;
    }

    i32  compile_status;
    char compile_log[512];

    u32 vertex_handle;
    u32 fragment_handle;

    {   // Compile the vertex part
        vertex_handle = glCreateShader(GL_VERTEX_SHADER);

        char *vertex_strings[3] = {
            "#version 330\n",
            "#define VERTEX\n",
            shader_source.data
        };

        glShaderSource(vertex_handle, 3, vertex_strings, NULL);
        glCompileShader(vertex_handle);

        SHADER_COMPILATION_CHECK(vertex_handle, compile_status, compile_log, 512, "Vertex");

        if (!compile_status) {
            glDeleteShader(vertex_handle);
            free_array(shader_source);
            return false;
        }
    }

    {   // Compile the fragment part
        fragment_handle = glCreateShader(GL_FRAGMENT_SHADER);

        char *fragment_strings[3] = {
            "#version 330\n",
            "#define FRAGMENT\n",
            shader_source.data
        };

        glShaderSource(fragment_handle, 3, fragment_strings, NULL);
        glCompileShader(fragment_handle);

        SHADER_COMPILATION_CHECK(fragment_handle, compile_status, compile_log, 512, "Fragment");

        if (!compile_status) {
            glDeleteShader(fragment_handle);
            free_array(shader_source);
            return false;
        }
    }

    shader = {};

    shader.handle = glCreateProgram();
    glAttachShader(shader.handle, vertex_handle);
    glAttachShader(shader.handle, fragment_handle);

    glLinkProgram(shader.handle);

    // Check link status
    {
        glGetProgramiv(shader.handle, GL_LINK_STATUS, &compile_status);

        if (!compile_status) {
            glGetProgramInfoLog(shader.handle, 512, NULL, compile_log);

            log_print("Program link error!\n");
            log_print("%s\n", compile_log);

            glDeleteShader(vertex_handle);
            glDeleteShader(fragment_handle);

            glDeleteProgram(shader.handle);

            free_array(shader_source);

            return false;
        }
    }

    glDetachShader(shader.handle, vertex_handle);
    glDetachShader(shader.handle, fragment_handle);

    glDeleteShader(vertex_handle);
    glDeleteShader(fragment_handle);

    free_array(shader_source);

    log_print("Loaded shader: %s\n", path);

    return true;
}

internal void bind_mesh_buffer_objects(Mesh &mesh) {
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    {   // Index data
        glGenBuffers(1, &mesh.ebo);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indicies.length * sizeof(u32), mesh.indicies.data, GL_STATIC_DRAW);
    }

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

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}