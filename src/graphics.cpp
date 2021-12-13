internal bool load_program(const char *path, Program &shader, bool geometry = false, u32 define_count = 0, char **defines = nullptr) {
    Array<char> shader_source;

    bool success = read_whole_file(path, shader_source);

    if (!success) {
        return false;
    }

    i32  compile_status;
    char compile_log[512];

    u32 vertex_handle;
    u32 fragment_handle;
    u32 geometry_handle = UINT32_MAX;

    Array<char *> shader_inputs;
    allocate_array(shader_inputs, 16);

    shader_inputs.add("#version 420\n");
    for (u32 i = 0; i < define_count; i++) shader_inputs.add(defines[i]);

    {   // Compile the vertex part
        vertex_handle = glCreateShader(GL_VERTEX_SHADER);

        shader_inputs.add("#define VERTEX\n");
        shader_inputs.add(shader_source.data);

        glShaderSource(vertex_handle, (i32)shader_inputs.length, shader_inputs.data, NULL);
        glCompileShader(vertex_handle);

        shader_inputs.length -= 2;

        SHADER_COMPILATION_CHECK(vertex_handle, compile_status, compile_log, 512, "Vertex");

        if (!compile_status) {
            glDeleteShader(vertex_handle);
            free_array(shader_source);
            free_array(shader_inputs);
            return false;
        }
    }

    char cascade_split_define[32];
    snprintf(cascade_split_define, 32, "#define CASCADE_COUNT %d\n", SHADOW_CASCADE_COUNT);

    {   // Compile the fragment part
        fragment_handle = glCreateShader(GL_FRAGMENT_SHADER);

        shader_inputs.add("#define FRAGMENT\n");
        shader_inputs.add(cascade_split_define);
        shader_inputs.add(shader_source.data);

        glShaderSource(fragment_handle, (i32)shader_inputs.length, shader_inputs.data, NULL);
        glCompileShader(fragment_handle);

        shader_inputs.length -= 3;

        SHADER_COMPILATION_CHECK(fragment_handle, compile_status, compile_log, 512, "Fragment");

        if (!compile_status) {
            glDeleteShader(fragment_handle);
            free_array(shader_source);
            free_array(shader_inputs);
            return false;
        }
    }

    if (geometry) {
        geometry_handle = glCreateShader(GL_GEOMETRY_SHADER);

        shader_inputs.add("#define GEOMETRY\n");
        shader_inputs.add(cascade_split_define);
        shader_inputs.add(shader_source.data);

        glShaderSource(geometry_handle, (i32)shader_inputs.length, shader_inputs.data, NULL);
        glCompileShader(geometry_handle);

        shader_inputs.length -= 3;

        SHADER_COMPILATION_CHECK(geometry_handle, compile_status, compile_log, 512, "Geometry");

        if (!compile_status) {
            glDeleteShader(geometry_handle);
            free_array(shader_source);
            free_array(shader_inputs);
            return false;
        }
    }

    shader = {};

    shader.handle = glCreateProgram();
    glAttachShader(shader.handle, vertex_handle);
    glAttachShader(shader.handle, fragment_handle);

    if (geometry && geometry_handle != UINT32_MAX) glAttachShader(shader.handle, geometry_handle);

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
            free_array(shader_inputs);

            return false;
        }
    }

    glDetachShader(shader.handle, vertex_handle);
    glDetachShader(shader.handle, fragment_handle);

    glDeleteShader(vertex_handle);
    glDeleteShader(fragment_handle);

    free_array(shader_source);
    free_array(shader_inputs);

    if (define_count > 0 && shader.defines.length == 0) {
        allocate_array(shader.defines, define_count);

        for (u32 i = 0; i < define_count; i++) shader.defines.add(defines[i]);
    }

    shader.has_geometry = geometry;
    shader.source_file_hash = _catalog_hash(path);

    log_print("Loaded shader: %s (handle: %d)\n", path, shader.handle);

    return true;
}

internal void bind_mesh_buffer_objects(Mesh &mesh) {
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    // Index data
    if (mesh.indicies.length > 0) {
        glGenBuffers(1, &mesh.ebo);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indicies.length * sizeof(u32), mesh.indicies.data, GL_STATIC_DRAW);
    }

    i32 attrib = 0;

    {   // Vertex data
        glGenBuffers(1, &mesh.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
        glBufferData(GL_ARRAY_BUFFER, mesh.verticies.length * sizeof(Vector3), mesh.verticies.data, GL_STATIC_DRAW);

        glVertexAttribPointer(attrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(attrib);

        attrib += 1;
    }

    // Normal data
    if (mesh.normals.length > 0) {
        glGenBuffers(1, &mesh.nbo);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.nbo);
        glBufferData(GL_ARRAY_BUFFER, mesh.normals.length * sizeof(Vector3), mesh.normals.data, GL_STATIC_DRAW);

        glVertexAttribPointer(attrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(attrib);

        attrib += 1;
    }

    // UV data
    if (mesh.uvs.length > 0) {
        glGenBuffers(1, &mesh.tbo);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.tbo);
        glBufferData(GL_ARRAY_BUFFER, mesh.uvs.length * sizeof(Vector2), mesh.uvs.data, GL_STATIC_DRAW);

        glVertexAttribPointer(attrib, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(attrib);

        attrib += 1;
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

internal Mesh create_mesh_2d_quad(f32 half_size = 0.5f) {
    Mesh quad = {};

    allocate_array(quad.verticies, 4);
    allocate_array(quad.uvs,       4);
    allocate_array(quad.indicies,  6);

    quad.verticies.add(make_vector3(-half_size,  half_size, half_size));
    quad.verticies.add(make_vector3( half_size,  half_size, half_size));
    quad.verticies.add(make_vector3( half_size, -half_size, half_size));
    quad.verticies.add(make_vector3(-half_size, -half_size, half_size));

    quad.uvs.add(make_vector2(0.0f, 1.0f));
    quad.uvs.add(make_vector2(1.0f, 1.0f));
    quad.uvs.add(make_vector2(1.0f, 0.0f));
    quad.uvs.add(make_vector2(0.0f, 0.0f));

    quad.indicies.add(1);
    quad.indicies.add(0);
    quad.indicies.add(3);
    quad.indicies.add(3);
    quad.indicies.add(2);
    quad.indicies.add(1);

    bind_mesh_buffer_objects(quad);

    return quad;
}

internal Mesh create_skybox_cube() {
    Mesh cube = {};

    allocate_array(cube.verticies, 36);
    allocate_array(cube.indicies, 36);

    cube.verticies.add({ -1.0f,  1.0f, -1.0f });
    cube.verticies.add({ -1.0f, -1.0f, -1.0f });
    cube.verticies.add({  1.0f, -1.0f, -1.0f });
    cube.verticies.add({  1.0f, -1.0f, -1.0f });
    cube.verticies.add({  1.0f,  1.0f, -1.0f });
    cube.verticies.add({ -1.0f,  1.0f, -1.0f });

    cube.verticies.add({ -1.0f, -1.0f,  1.0f });
    cube.verticies.add({ -1.0f, -1.0f, -1.0f });
    cube.verticies.add({ -1.0f,  1.0f, -1.0f });
    cube.verticies.add({ -1.0f,  1.0f, -1.0f });
    cube.verticies.add({ -1.0f,  1.0f,  1.0f });
    cube.verticies.add({ -1.0f, -1.0f,  1.0f });

    cube.verticies.add({ 1.0f, -1.0f, -1.0f });
    cube.verticies.add({ 1.0f, -1.0f,  1.0f });
    cube.verticies.add({ 1.0f,  1.0f,  1.0f });
    cube.verticies.add({ 1.0f,  1.0f,  1.0f });
    cube.verticies.add({ 1.0f,  1.0f, -1.0f });
    cube.verticies.add({ 1.0f, -1.0f, -1.0f });

    cube.verticies.add({ -1.0f, -1.0f,  1.0f });
    cube.verticies.add({ -1.0f,  1.0f,  1.0f });
    cube.verticies.add({  1.0f,  1.0f,  1.0f });
    cube.verticies.add({  1.0f,  1.0f,  1.0f });
    cube.verticies.add({  1.0f, -1.0f,  1.0f });
    cube.verticies.add({ -1.0f, -1.0f,  1.0f });

    cube.verticies.add({ -1.0f,  1.0f, -1.0f });
    cube.verticies.add({  1.0f,  1.0f, -1.0f });
    cube.verticies.add({  1.0f,  1.0f,  1.0f });
    cube.verticies.add({  1.0f,  1.0f,  1.0f });
    cube.verticies.add({ -1.0f,  1.0f,  1.0f });
    cube.verticies.add({ -1.0f,  1.0f, -1.0f });

    cube.verticies.add({-1.0f, -1.0f, -1.0f });
    cube.verticies.add({-1.0f, -1.0f,  1.0f });
    cube.verticies.add({ 1.0f, -1.0f, -1.0f });
    cube.verticies.add({ 1.0f, -1.0f, -1.0f });
    cube.verticies.add({-1.0f, -1.0f,  1.0f });
    cube.verticies.add({ 1.0f, -1.0f,  1.0  });

    for (i32 i = 0; i < cube.indicies.capacity; i++) cube.indicies.add(i);

    bind_mesh_buffer_objects(cube);

    return cube;
}