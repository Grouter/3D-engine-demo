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

    Mesh *result = game_state.resources.meshes.get(index);

    return result;
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
internal void load_model(const char *name, Mesh &mesh) {
    log_print("Loading %s 3D model\n", name);

    mesh = {};

    tinyobj::ObjReader reader;

    bool result = reader.ParseFromFile(name);
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

    log_print("Mesh sub meshes: %llu\n", mesh.sub_meshes.length);
    log_print("Mesh verticies:  %llu\n", mesh.verticies.length);
    log_print("Mesh normals:    %llu\n", mesh.normals.length);
    log_print("Mesh indicies:   %llu\n", mesh.indicies.length);
    log_print("Mesh uv:         %llu\n", mesh.uvs.length);

    bind_mesh_buffer_objects(mesh);
}

internal void load_mesh_file(const char *override_name = nullptr) {
    const char *name = override_name ? override_name : "mesh.resources";

    Resources *resources = &game_state.resources;

    Array<char> buffer = {};
    read_whole_file(name, buffer);

    std::string mesh_name;
    Mesh mesh = {};
    u64 sub_mesh_index = 0;

    std::string material_name;

    char *walker = buffer.data;

    while(walker[0] > 0) {
        eat_whitespace(&walker);

        if (walker[0] == 0)
            break;

        if (walker[0] == '#') {
            eat_until(&walker, '\n');
            continue;
        }

        if (walker[0] == '!') { // New mesh!

            // We already parsed a mesh, save it!
            if (mesh_name.length() != 0) {
                log_print("Loaded mesh: %s\n", mesh_name.c_str());

                resources->meshes.add(mesh);

                u64 mesh_index = resources->meshes.length - 1;

                catalog_put(resources->mesh_catalog, mesh_name.c_str(), mesh_index);
            }

            mesh_name.clear();
            mesh = {};
            sub_mesh_index = 0;

            walker++;   // Skip '!' char
            eat_whitespace(&walker);

            // Parse name
            {
                u64 mesh_name_len = word_length(walker);
                mesh_name.append(walker, mesh_name_len);
                walker += mesh_name_len;
            }

            eat_whitespace(&walker);

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

        resources->meshes.add(mesh);

        u64 mesh_index = resources->meshes.length - 1;

        catalog_put(resources->mesh_catalog, mesh_name.c_str(), mesh_index);
    }
}

internal void load_material_file(const char *override_name = nullptr) {
    const char *name = override_name ? override_name : "material.resources";

    Resources *resources = &game_state.resources;

    Array<char> buffer = {};
    read_whole_file(name, buffer);

    std::string material_name;
    Material material = {};

    char *walker = buffer.data;

    while(walker[0] > 0) {
        eat_whitespace(&walker);

        if (walker[0] == 0)
            break;

        if (walker[0] == '#') {
            eat_until(&walker, '\n');
            continue;
        }

        if (walker[0] == '!') { // New material!

            // We already parsed a material, save it!
            if (material_name.length() != 0) {
                log_print("Loaded material: %s\n", material_name.c_str());

                resources->materials.add(material);

                u64 material_index = resources->materials.length - 1;

                catalog_put(resources->material_catalog, material_name.c_str(), material_index);
            }

            material_name.clear();

            walker++;   // Skip '!' char
            eat_whitespace(&walker);

            u64 material_name_len = word_length(walker);
            material_name.append(walker, material_name_len);

            material = {};
            material.texture = resources->textures.data[0]; // Default texture;

            walker += material_name_len;

            continue;
        }

        u64 attr_name_len = find(walker, ':');

        if (strncmp(walker, "color", attr_name_len) == 0) {
            walker += attr_name_len + 1;
            eat_whitespace(&walker);

            sscanf(walker, "%f %f %f", &material.color.x, &material.color.y, &material.color.z);

            eat_until(&walker, '\n');
        }
        else if (strncmp(walker, "texture", attr_name_len) == 0) {
            walker += attr_name_len + 1;
            eat_whitespace(&walker);

            u64 texture_name_len = word_length(walker);

            std::string texture_name;

            texture_name.append(walker, texture_name_len);

            u32 texture;

            if (catalog_cointains(game_state.resources.texture_catalog, texture_name.c_str())) {
                texture = get_texture(texture_name.c_str());
            }
            else {
                texture = load_texture(texture_name.c_str());
            }

            material.texture = texture;

            eat_until(&walker, '\n');
        }
    }

    // Save the last parsed material
    {
        log_print("Loaded material: %s\n", material_name.c_str());

        resources->materials.add(material);

        u64 material_index = resources->materials.length - 1;

        catalog_put(resources->material_catalog, material_name.c_str(), material_index);
    }
}

internal void init_resources(Resources &resources) {
    allocate_resource_catalog(resources.shader_catalog, 5);

    // Default shader
    {
        bool status = load_program("shaders/default.glsl", resources.programs[0]);
        if (!status) {
            exit(1);
        }
        catalog_put(resources.shader_catalog, "default.glsl", 0);
    }

    // Meshes
    allocate_array(resources.meshes, 50);
    allocate_resource_catalog(resources.mesh_catalog, 50);

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

        default_mat.color = make_vector3(1.0f, 1.0f, 1.0f);
        default_mat.texture = resources.textures.data[0];

        resources.materials.add(default_mat);
        catalog_put(resources.material_catalog, "default", 0);
    }

    // @Todo: Error material? This material would be returned
    // when user tries to find a non-existing material.
}

internal void unload_meshes() {
    Mesh *it;

    array_foreach(game_state.resources.meshes, it) {
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
    }

    game_state.resources.meshes.clear();
    catalog_clear(game_state.resources.mesh_catalog);
}

// Unloads all texture except the first default one!
internal void unload_textures() {
    for (u64 i = 0; i < game_state.resources.textures.length; i++) {
        glDeleteTextures(1, &game_state.resources.textures.data[i]);
    }

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