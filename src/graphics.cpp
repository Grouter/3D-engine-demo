internal Program load_program(const char *name) {
    Program program;

    FILE *shader_file = fopen(name, "r");
    if (!shader_file) {
        printf("Error opening a shader file: %s\n", name);
        exit(1);
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

    free(shader_source);

    return program;
}

internal void bind_mesh(Mesh &mesh) {
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    {   // Vertex data
        glGenBuffers(1, &mesh.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertex_count * sizeof(VertexP), mesh.verticies, GL_STATIC_DRAW);
    }

    {   // Index data
        glGenBuffers(1, &mesh.ebo);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.index_count * sizeof(u32), mesh.indicies, GL_STATIC_DRAW);
    }

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

internal Mesh create_rectangle(u32 w, u32 h) {
    Mesh mesh;

    f32 w_half = (f32)w / 2.0f;
    f32 h_half = (f32)h / 2.0f;

    mesh.verticies = new VertexP[] {
        VertexP { make_vector3(-w_half,  h_half, 0.0f) },
        VertexP { make_vector3( w_half,  h_half, 0.0f) },
        VertexP { make_vector3( w_half, -h_half, 0.0f) },
        VertexP { make_vector3(-w_half, -h_half, 0.0f) },
    };

    mesh.indicies = new u32[] {
        1, 0, 3,
        3, 2, 1
    };

    mesh.vertex_count = 4;
    mesh.index_count = 6;

    bind_mesh(mesh);

    return mesh;
}

internal Mesh create_cube(f32 size) {
    f32 half_size = size / 2.0f;

    Mesh mesh = {};

    mesh.verticies = new VertexP[] {
        VertexP { make_vector3(-half_size,  half_size, half_size) },
        VertexP { make_vector3( half_size,  half_size, half_size) },
        VertexP { make_vector3( half_size, -half_size, half_size) },
        VertexP { make_vector3(-half_size, -half_size, half_size) },

        VertexP { make_vector3(-half_size,  half_size, -half_size) },
        VertexP { make_vector3( half_size,  half_size, -half_size) },
        VertexP { make_vector3( half_size, -half_size, -half_size) },
        VertexP { make_vector3(-half_size, -half_size, -half_size) },
    };

    mesh.indicies = new u32[] {
         // Front
        1, 0, 3,
        3, 2, 1,

        // Left
        5, 1, 2,
        2, 6, 5,

        // Back
        4, 5, 6,
        6, 7, 4,

        // Right
        0, 4, 7,
        7, 3, 0,

        // Top
        5, 4, 0,
        0, 1, 5,

        // Bottom
        2, 3, 7,
        7, 6, 2,
    };

    mesh.vertex_count = 8;
    mesh.index_count  = 36;

    bind_mesh(mesh);

    return mesh;
}