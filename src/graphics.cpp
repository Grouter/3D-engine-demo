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

internal Mesh create_cube(f32 size) {
    f32 half_size = size / 2.0f;

    Mesh mesh = {};

    allocate_array(mesh.verticies, 8);

    mesh.verticies.add(make_vector3(-half_size,  half_size, half_size));
    mesh.verticies.add(make_vector3( half_size,  half_size, half_size));
    mesh.verticies.add(make_vector3( half_size, -half_size, half_size));
    mesh.verticies.add(make_vector3(-half_size, -half_size, half_size));
    mesh.verticies.add(make_vector3(-half_size,  half_size, -half_size));
    mesh.verticies.add(make_vector3( half_size,  half_size, -half_size));
    mesh.verticies.add(make_vector3( half_size, -half_size, -half_size));
    mesh.verticies.add(make_vector3(-half_size, -half_size, -half_size));

    // This is hardcoded array initialization... we can do better
    mesh.indicies.data = new u32[] {
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
    mesh.indicies.length = 36;
    mesh.indicies.capacity = 36;

    bind_mesh_buffer_objects(mesh);

    return mesh;
}

// @Todo: This mesh loading is wasting a lot of memory!
// We allocate the same amount of verticies and normals as we have indicies.
// I think some of the data is always duplicate (which wastes memory)...
// We should actually backcheck if the vertex already exists and if not, only then add it.
internal Mesh load_model(const char *name) {
    printf("Loading %s 3D model\n", name);

    tinyobj::ObjReader reader;

    bool result = reader.ParseFromFile(name);
    if (!result) {
        printf("Error loading %s 3D model file\n", name);
        if (!reader.Error().empty()) {
            printf(reader.Error().c_str());
        }
    }

    if (!reader.Warning().empty()) {
        printf(reader.Warning().c_str());
    }

    tinyobj::attrib_t              data   = reader.GetAttrib();
    std::vector<tinyobj::shape_t>  shapes = reader.GetShapes();

    tinyobj::mesh_t *sub_mesh;

    u64 index_count = 0;

    // This just counts all the indicies of all sub meshes so we can
    // allocate space for them. Using a dynamic array would be very slow...
    for (u64 shape_index = 0; shape_index < shapes.size(); shape_index++) {
        sub_mesh = &shapes[shape_index].mesh;

        // We export .obj with triangulate faces option, so every face has exactly 3 verticies.
        u64 sub_mesh_indicies = sub_mesh->num_face_vertices.size() * 3;
        index_count += sub_mesh_indicies;
    }

    Mesh mesh = {};

    allocate_array(mesh.sub_meshes, shapes.size());
    allocate_array(mesh.verticies,  index_count);
    allocate_array(mesh.normals,    index_count);
    allocate_array(mesh.indicies,   index_count);
    allocate_array(mesh.uvs,        index_count);

    // Indicies and normals
    for (u64 shape_index = 0; shape_index < shapes.size(); shape_index++) {
        u64 index_offset = 0;
        sub_mesh = &shapes[shape_index].mesh;

        SubMeshInfo sub_mesh_info = {};

        sub_mesh_info.first_index = (u32)mesh.indicies.length;
        sub_mesh_info.index_count = (u32)sub_mesh->indices.size();

        mesh.sub_meshes.add(sub_mesh_info);

        for (u64 f = 0; f < sub_mesh->num_face_vertices.size(); f++) {
            u64 fv = (u64)sub_mesh->num_face_vertices[f];

            for (u64 v = 0; v < fv; v++) {
                tinyobj::index_t idx = sub_mesh->indices[index_offset + v];

                // Indicies
                mesh.indicies.add((u32)(index_offset + v));

                // Position
                {
                    Vector3 position = {};

                    position.x = data.vertices[3 * (u64)idx.vertex_index + 0];
                    position.y = data.vertices[3 * (u64)idx.vertex_index + 1];
                    position.z = data.vertices[3 * (u64)idx.vertex_index + 2];

                    mesh.verticies.add(position);
                }

                // Normals
                if (idx.normal_index >= 0) {
                    Vector3 normal = {};

                    normal.x = data.normals[3 * (u64)idx.normal_index + 0];
                    normal.y = data.normals[3 * (u64)idx.normal_index + 1];
                    normal.z = data.normals[3 * (u64)idx.normal_index + 2];

                    mesh.normals.add(normal);
                }

                // UVs
                if (idx.texcoord_index >= 0) {
                    Vector2 uv = {};

                    uv.x = data.texcoords[2 * (u64)idx.texcoord_index + 0];
                    uv.y = data.texcoords[2 * (u64)idx.texcoord_index + 1];

                    mesh.uvs.add(uv);
                }
            }

            index_offset += fv;
        }
    }

    printf("Mesh sub meshes: %llu\n", mesh.sub_meshes.length);
    printf("Mesh verticies:  %llu\n", mesh.verticies.length);
    printf("Mesh normals:    %llu\n", mesh.normals.length);
    printf("Mesh indicies:   %llu\n", mesh.indicies.length);
    printf("Mesh uv:         %llu\n", mesh.uvs.length);

    bind_mesh_buffer_objects(mesh);

    return mesh;
}