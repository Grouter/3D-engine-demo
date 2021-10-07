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

    bind_mesh_buffer_objects(mesh);

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

    bind_mesh_buffer_objects(mesh);

    return mesh;
}

internal Mesh load_model(const char *name) {
    printf("Loading %s 3D model\n", name);

    tinyobj::ObjReader reader;

    bool result = reader.ParseFromFile(name);
    if (!result) {
        printf("Error loading %s 3D model file\n", name);
    }

    tinyobj::attrib_t              data   = reader.GetAttrib();
    std::vector<tinyobj::shape_t>  shapes = reader.GetShapes();

    tinyobj::mesh_t *sub_mesh;

    Mesh mesh = {};
    mesh.vertex_count = (u32)data.vertices.size() / 3;

    // This just counts all the indicies of all sub meshes
    for (u64 shape_index = 0; shape_index < shapes.size(); shape_index++) {
        sub_mesh = &shapes[shape_index].mesh;

        for (u64 f = 0; f < sub_mesh->num_face_vertices.size(); f++) {
            u64 face_verticies = (u64)sub_mesh->num_face_vertices[f];

            mesh.index_count += (u32)face_verticies;
        }
    }

    printf("Mesh verticies: %lu\n", mesh.vertex_count);
    printf("Mesh indicies: %lu\n", mesh.index_count);

    mesh.verticies = (VertexP*)malloc(mesh.vertex_count * sizeof(VertexP));
    mesh.indicies  = (u32*)malloc(mesh.index_count * sizeof(u32));

    printf("Mesh buffer initialized: %u %u\n", mesh.vertex_count, mesh.index_count);

    // Copy loaded data to our mesh data
    // @Speed: this iterates over all indicies...
    {
        memcpy(mesh.verticies, data.vertices.data(), data.vertices.size() * sizeof(f32));

        u32 stored_indicies = 0;

        for (u64 shape_index = 0; shape_index < shapes.size(); shape_index++) {
            u64 index_offset = 0;
            sub_mesh = &shapes[shape_index].mesh;

            for (u64 f = 0; f < sub_mesh->num_face_vertices.size(); f++) {
                u64 fv = (u64)sub_mesh->num_face_vertices[f];

                for (u64 v = 0; v < fv; v++) {
                    mesh.indicies[stored_indicies] = sub_mesh->indices[index_offset + v].vertex_index;
                    stored_indicies += 1;
                }

                index_offset += fv;
            }
        }
    }

    bind_mesh_buffer_objects(mesh);

    return mesh;
}