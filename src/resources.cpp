internal u64 get_material_index(const char *name) {
    u64 index = catalog_get(game_state.resources.material_catalog, name);

    if (index == UINT64_MAX) {
        log_print("Couldn't find a material with name: %s\n", name);
        index = 0;   // Set to default material
    }

    return index;
}

internal u32 get_texture(const char *name) {
    u64 index = catalog_get(game_state.resources.texture_catalog, name);

    if (index == UINT64_MAX) {
        log_print("Couldn't find a texture with name: %s\n", name);
        index = 0;  // Set to white texture
    }

    u32 texture = game_state.resources.textures.data[index];

    return texture;
}

internal Mesh *get_mesh(const char *name) {
    u64 index = catalog_get(game_state.resources.mesh_catalog, name);

    if (index == UINT64_MAX) {
        log_print("Couldn't find a mesh with name: %s\n", name);
        return nullptr;
    }

    Mesh *result = &game_state.resources.meshes[index];

    return result;
}

inline Font &get_font(FontResource font_resource) {
    Font &font = game_state.resources.fonts[font_resource];

    return font;
}

internal u32 load_texture(const char *image) {
    std::string path = "textures/";
    path.append(image);

    u32 texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);

    i32 width, height, nr_channels;
    u8 *data = stbi_load(path.c_str(), &width, &height, &nr_channels, 0);

    if (data) {
        if (nr_channels == 3) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else if (nr_channels == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        else {
            log_print("Unsupported texture channels number\n");
        }

        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        log_print("Error loading texture.\n");
    }

    stbi_image_free(data);

    if (catalog_cointains(game_state.resources.texture_catalog, image)) {
        u32 *old_texture = (u32 *)catalog_get(game_state.resources.texture_catalog, image);
        u64 old_texture_index = old_texture - game_state.resources.textures.data;

        game_state.resources.textures.data[old_texture_index] = texture;
    }
    else {
        game_state.resources.textures.add(texture);

        u64 texture_index = game_state.resources.textures.length - 1;

        catalog_put(game_state.resources.texture_catalog, image, texture_index);
    }

    return texture;
}

internal u32 create_white_texture() {
    u32 texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    u8 data[4] = { 255, 255, 255, 255 };

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glGenerateMipmap(GL_TEXTURE_2D);

    return texture;
}

// @Todo: This mesh loading is wasting a lot of memory!
// We allocate the same amount of verticies and normals as we have indicies.
// I think some of the data is always duplicate (which wastes memory)...
// We should actually backcheck if the vertex already exists and if not, only then add it.
// @Broken: multi-submesh meshes are not loaded properly!
internal void load_model(const char *name, Mesh &mesh) {
    log_print("Loading %s 3D model\n", name);

    mesh = {};

    tinyobj::ObjReader reader;
    tinyobj::ObjReaderConfig reader_config = {};
    reader_config.triangulate = false;
    reader_config.vertex_color = true;

    bool result = reader.ParseFromFile(name, reader_config);
    if (!result) {
        log_print("Error loading %s 3D model file\n", name);
        if (!reader.Error().empty()) {
            log_print(reader.Error().c_str());
        }
    }

    if (!reader.Warning().empty()) {
        log_print(reader.Warning().c_str());
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

    allocate_array(mesh.sub_meshes, shapes.size());
    allocate_array(mesh.verticies,  index_count);
    allocate_array(mesh.normals,    index_count);
    allocate_array(mesh.indicies,   index_count);
    allocate_array(mesh.uvs,        index_count);

    for (u64 shape_index = 0; shape_index < shapes.size(); shape_index++) {
        u64 index_offset = 0;
        sub_mesh = &shapes[shape_index].mesh;

        SubMeshInfo sub_mesh_info = {};

        sub_mesh_info.first_index = (u32)mesh.indicies.length;
        sub_mesh_info.index_count = (u32)sub_mesh->num_face_vertices.size() * 3;
        sub_mesh_info.material_index = get_material_index("color_palette");

        mesh.sub_meshes.add(sub_mesh_info);

        for (u64 f = 0; f < sub_mesh->num_face_vertices.size(); f++) {
            u64 fv = (u64)sub_mesh->num_face_vertices[f];

            for (u64 v = 0; v < fv; v++) {
                tinyobj::index_t idx = sub_mesh->indices[index_offset + v];

                // Indicies
                mesh.indicies.add((u32)(mesh.indicies.length));

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

#if 0
    log_print("Mesh sub meshes: %llu\n", mesh.sub_meshes.length);
    log_print("Mesh verticies:  %llu\n", mesh.verticies.length);
    log_print("Mesh normals:    %llu\n", mesh.normals.length);
    log_print("Mesh indicies:   %llu\n", mesh.indicies.length);
    log_print("Mesh uv:         %llu\n", mesh.uvs.length);
#endif

    bind_mesh_buffer_objects(mesh);
}

internal bool load_mesh_file(const char *override_name = nullptr) {
    const char *name = override_name ? override_name : "mesh.resources";

    Resources *resources = &game_state.resources;

    Array<char> buffer = {};
    bool success = read_whole_file(name, buffer);
    if (!success) {
        return false;
    }

    std::string mesh_name;
    Mesh mesh = {};
    u64 sub_mesh_index = 0;

    std::string material_name;

    char *walker = buffer.data;

    while(walker[0] > 0) {
        walker = eat_whitespace(walker);

        if (walker[0] == 0)
            break;

        if (walker[0] == '#') {
            walker = eat_until(walker, '\n');
            continue;
        }

        if (walker[0] == '!') { // New mesh!

            // We already parsed a mesh, save it!
            if (mesh_name.length() != 0) {
                log_print("Loaded mesh: %s\n", mesh_name.c_str());

                mesh.loaded = true;

                resources->meshes.add(mesh);

                u64 mesh_index = resources->meshes.length - 1;

                catalog_put(resources->mesh_catalog, mesh_name.c_str(), mesh_index);
            }

            mesh_name.clear();
            mesh = {};
            sub_mesh_index = 0;

            walker++;   // Skip '!' char
            walker = eat_whitespace(walker);

            // Parse name
            {
                u64 mesh_name_len = word_length(walker);
                mesh_name.append(walker, mesh_name_len);
                walker += mesh_name_len;
            }

            walker = eat_whitespace(walker);

            // Parse mesh file
            {
                u64 mesh_file_name_len = word_length(walker);
                std::string mesh_file;
                mesh_file.append(walker, mesh_file_name_len);

                load_model(mesh_file.c_str(), mesh);
                // @Todo: check if mesh is loaded!

                walker += mesh_file_name_len;
            }

            continue;
        }

        u64 material_name_len = word_length(walker);
        material_name.clear();
        material_name.append(walker, material_name_len);

        if (sub_mesh_index < mesh.sub_meshes.length) {
            mesh.sub_meshes.data[sub_mesh_index].material_index = get_material_index(material_name.c_str());
            sub_mesh_index += 1;
        }
        else {
            log_print("Too much materials for mesh: %s (skipping material %s)\n", mesh_name.c_str(), material_name.c_str());
        }

        walker += material_name_len;
    }

    // Save the last parsed mesh
    {
        log_print("Loaded mesh: %s\n", mesh_name.c_str());

        mesh.loaded = true;

        resources->meshes.add(mesh);

        u64 mesh_index = resources->meshes.length - 1;

        catalog_put(resources->mesh_catalog, mesh_name.c_str(), mesh_index);
    }

    return true;
}

internal bool load_material_file(const char *override_name = nullptr) {
    const char *name = override_name ? override_name : "material.resources";

    Resources *resources = &game_state.resources;

    Array<char> buffer;

    bool success = read_whole_file(name, buffer);
    if (!success) return false;

    Material material;

    Array<VariableBinding> variables;
    allocate_array(variables, 4);

    char *material_name_start;
    char *walker = buffer.data;

    while (*walker) {
        walker = parse_file_entry(variables, &material_name_start, walker);

        material = {};
        material.texture = get_texture("white");

        VariableBinding *it;
        array_foreach(variables, it) {
            if (strcmp(it->name, "color") == 0) {
                material.color = it->value.vector4_value;
            }
            else if (strcmp(it->name, "texture") == 0) {
                u32 texture;

                if (catalog_cointains(resources->texture_catalog, it->value.string_value)) {
                    texture = get_texture(it->value.string_value);
                }
                else {
                    texture = load_texture(it->value.string_value);
                }

                material.texture = texture;
            }
        }

        // Save material
        {
            u32 material_name_length = word_length(material_name_start);
            log_print("Loaded material: %.*s\n", material_name_length, material_name_start);

            char *material_name = copy_string(material_name_start, material_name_length);

            resources->materials.add(material);

            u64 material_index = resources->materials.length - 1;
            catalog_put(resources->material_catalog, material_name, material_index);

            free(material_name);
        }

        variables.clear();
    }

    free_array(variables);
    free_array(buffer);

    return true;
}

internal bool load_world_file(EntityStorage &storage) {
    Array<char> buffer;

    bool success = read_whole_file("island.world", buffer);
    if (!success) return false;

    Array<VariableBinding> variables;
    allocate_array(variables, 4);

    char *entry_type;
    char *walker = buffer.data;

    while (*walker) {
        walker = parse_file_entry(variables, &entry_type, walker);

        if (strncmp(entry_type, "entity", 6) == 0) {
            EntityType type_of_new_entity = EntityType_BASIC;

            // Find type
            {
                VariableBinding *it;
                array_foreach(variables, it) {
                    if (strcmp(it->name, "type") == 0) {
                        type_of_new_entity = (EntityType)it->value.integer_value;
                        break;
                    }
                }
            }

            Entity *new_entity = create_entity_from_type(storage, type_of_new_entity);
            new_entity->program = &game_state.resources.programs[0];

            if (!new_entity) {
                log_print("Invalid entity type in world save file\n");
                variables.clear();
                continue;
            }

            VariableBinding *it;
            array_foreach(variables, it) {
                if (strcmp(it->name, "mesh") == 0) {
                    new_entity->mesh = get_mesh(it->value.string_value);
                }
                else if (strcmp(it->name, "position") == 0) {
                    new_entity->position = it->value.vector3_value;
                }
                else if (strcmp(it->name, "rotation") == 0) {
                    new_entity->rotation = it->value.vector3_value;
                }
                else if (strcmp(it->name, "scale") == 0) {
                    new_entity->scale = it->value.vector3_value;
                }
            }
        }
        else {
            log_print("Unknown world save file entry\n");
        }

        variables.clear();
    }

    free_array(variables);
    free_array(buffer);

    return true;
}

internal void init_resources(Resources &resources) {
    allocate_resource_catalog(resources.shader_catalog, ShaderResource_COUNT);

    // Default shader
    {
        bool status = load_program("shaders/default.glsl", resources.programs[ShaderResource_Default]);
        if (!status) exit(1);
        catalog_put(resources.shader_catalog, "default.glsl", ShaderResource_Default);
    }

    // Font shader
    {
        bool status = load_program("shaders/2d.glsl", resources.programs[ShaderResource_2D]);
        if (!status) exit(1);
        catalog_put(resources.shader_catalog, "2d.glsl", ShaderResource_2D);
    }

    // Shadow shader
    {
        bool status = load_program("shaders/shadow.glsl", resources.programs[ShaderResource_Shadow], true);
        if (!status) exit(1);
        catalog_put(resources.shader_catalog, "shadow.glsl", ShaderResource_Shadow);
    }

    // Skybox shader
    {
        bool status = load_program("shaders/skybox.glsl", resources.programs[ShaderResource_Skybox]);
        if (!status) exit(1);
        catalog_put(resources.shader_catalog, "skybox.glsl", ShaderResource_Skybox);
    }

    // HDR shader
    {
        bool status = load_program("shaders/hdr.glsl", resources.programs[ShaderResource_HDR]);
        if (!status) exit(1);
        catalog_put(resources.shader_catalog, "hdr.glsl", ShaderResource_HDR);
    }

    // Meshes
    allocate_array(resources.meshes, 50);
    allocate_resource_catalog(resources.mesh_catalog, 50);
    resources.meshes.add(create_mesh_2d_quad());
    resources.meshes.add(create_skybox_cube());
    resources.meshes.add(create_mesh_2d_quad(1.0f));

    // Textures
    allocate_array(resources.textures, 50);
    allocate_resource_catalog(resources.texture_catalog, 50);

    // Default white texture
    {
        resources.textures.add(create_white_texture());
        catalog_put(game_state.resources.texture_catalog, "white", 0);
    }

    // Materials
    allocate_array(resources.materials, 50);
    allocate_resource_catalog(resources.material_catalog, 50);

    // Default material (always first in the materials list)
    {
        Material default_mat = {};

        default_mat.color = make_vector4(1.0f, 1.0f, 1.0f, 1.0f);
        default_mat.texture = resources.textures.data[0];

        resources.materials.add(default_mat);
        catalog_put(resources.material_catalog, "default", 0);
    }

    // @Todo: Error material? This material would be returned
    // when user tries to find a non-existing material.

    // Fonts
    allocate_array(resources.fonts, 3);
    resources.fonts.add(load_font("karmina_regular.otf", FONT_SIZE_SMALL,  FontResource_Small));
    resources.fonts.add(load_font("karmina_regular.otf", FONT_SIZE_MEDIUM, FontResource_Medium));
    resources.fonts.add(load_font("karmina_regular.otf", FONT_SIZE_BIG,    FontResource_Big));

    // Animation
    resources.camera_animation = load_camera_animation("test.keyframes");

    // Skybox Cubemap
    {
        u32 cubemap;
        glGenTextures(1, &cubemap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_set_flip_vertically_on_load(false);

        char *sides[6] = {
            "cubemap/left.png",
            "cubemap/right.png",
            "cubemap/top.png",
            "cubemap/bottom.png",
            "cubemap/back.png",
            "cubemap/front.png",
        };

        i32 width, height, nr_channels;

        for (u32 i = 0; i < 6; i++) {
            stbi_set_flip_vertically_on_load(i == 2 || i == 3);

            u8 *data = stbi_load(sides[i], &width, &height, &nr_channels, 0);

            if (data) {
                i32 format = GL_RGB;
                if (nr_channels == 4) format = GL_RGBA;

                glTexImage2D(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data
                );

                stbi_image_free(data);
            }
            else {
                log_print("!!!!! Error loading a the cubemap %s side\n", sides[i]);
                stbi_image_free(data);
            }
        }

        game_state.skybox_cubemap = cubemap;
    }
}

internal void unload_meshes() {
    Mesh *it;

    // Stop at index 3 to not remove the first static meshes
    for (i64 i = game_state.resources.meshes.length - 1; i >= 3; i--) {
        it = &game_state.resources.meshes[i];

        glDeleteBuffers(1, &it->vao);
        glDeleteBuffers(1, &it->vbo);
        glDeleteBuffers(1, &it->nbo);
        glDeleteBuffers(1, &it->ebo);
        glDeleteBuffers(1, &it->tbo);

        free_array(it->sub_meshes);

        free_array(it->verticies);
        free_array(it->indicies);
        free_array(it->normals);
        free_array(it->uvs);

        it->loaded = false;

        game_state.resources.meshes.remove_last();
    }

    catalog_clear(game_state.resources.mesh_catalog);
}

// Unloads all texture except the first default one!
internal void unload_textures() {
    glDeleteTextures(
        (i32)game_state.resources.textures.length - 1,
        game_state.resources.textures.data + 1
    );

    game_state.resources.textures.clear();
    game_state.resources.textures.length = 1;   // This will keep the first white texture

    catalog_clear(game_state.resources.texture_catalog);
    catalog_put(game_state.resources.texture_catalog, "white", 0);
}

internal void unload_materials() {
    game_state.resources.materials.clear();
    game_state.resources.materials.length = 1;  // Keep the first default material

    catalog_clear(game_state.resources.material_catalog);
    catalog_put(game_state.resources.material_catalog, "default", 0);
}