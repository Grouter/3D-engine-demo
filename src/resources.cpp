internal void allocate_resource_catalog(ResourceCatalog &catalog, u64 size) {
    catalog.size = size;
    catalog.entries = (ResourceCatalog::Entry **)calloc(size, sizeof(ResourceCatalog::Entry *));
}

internal u64 _catalog_hash(const char *key) {
    if (!key || !key[0]) return 1;

    return key[0] * 33 * _catalog_hash(key + 1);
}

internal void catalog_put(ResourceCatalog &catalog, const char *key, u64 index) {
    u64 hashed_index = _catalog_hash(key) % catalog.size;
    u64 key_size = strlen(key);

    ResourceCatalog::Entry *entry = catalog.entries[hashed_index];

    if (entry == nullptr) {
        catalog.entries[hashed_index] = (ResourceCatalog::Entry *)malloc(sizeof(ResourceCatalog::Entry));

        entry = catalog.entries[hashed_index];

        entry->key = (char *)malloc((key_size + 1) * sizeof(char));
        strcpy_s(entry->key, key_size + 1, key);

        entry->index = index;
        entry->next_in_hash = nullptr;

        return;
    }

    while (1) {
        if (strcmp(entry->key, key) == 0) { // We just update the value.
            entry->index = index;
            return;
        }

        if (entry->next_in_hash == nullptr) {   // We found the last one!
            ResourceCatalog::Entry *new_entry = (ResourceCatalog::Entry *)malloc(sizeof(ResourceCatalog::Entry));

            new_entry->key = (char *)malloc((key_size + 1) * sizeof(char));
            strcpy_s(new_entry->key, key_size + 1, key);
            new_entry->index = index;
            new_entry->next_in_hash = nullptr;

            entry->next_in_hash = new_entry;

            return;
        }

        entry = entry->next_in_hash;
    }
}

internal void catalog_remove(ResourceCatalog &catalog, const char *key) {
    u64 hashed_index = _catalog_hash(key) % catalog.size;

    ResourceCatalog::Entry *walker = catalog.entries[hashed_index];

    if (walker == nullptr) {
        printf("Trying to remove unexistent entry (key: %s)\n", key);
        return;
    }

    if (strcmp(walker->key, key) == 0) {    // Removing the first entry
        catalog.entries[hashed_index] = catalog.entries[hashed_index]->next_in_hash;

        free(walker->key);
        free(walker);

        return;
    }

    ResourceCatalog::Entry *prev = walker;
    walker = walker->next_in_hash;

    while (walker) {
        if (strcmp(walker->key, key) == 0) {    // Remove!
            prev->next_in_hash = walker->next_in_hash;

            free(walker->key);
            free(walker);

            return;
        }

        prev = walker;
        walker = walker->next_in_hash;
    }

    printf("Trying to remove unexistent entry (key: %s)\n", key);
}

internal u64 catalog_get(ResourceCatalog &catalog, const char *key) {
    ResourceCatalog::Entry *walker;

    for (u64 i = 0; i < catalog.size; i++) {
        walker = catalog.entries[i];

        while(walker) {
            if (strcmp(walker->key, key) == 0) {
                return walker->index;
            }

            walker = walker->next_in_hash;
        }
    }

    return UINT64_MAX;
}

internal bool catalog_cointains(ResourceCatalog &catalog, const char *key) {
    ResourceCatalog::Entry *walker;

    for (u64 i = 0; i < catalog.size; i++) {
        walker = catalog.entries[i];

        while(walker) {
            if (strcmp(walker->key, key) == 0) {
                return true;
            }

            walker = walker->next_in_hash;
        }
    }

    return false;
}

internal void catalog_dump(ResourceCatalog &catalog) {
    for (u64 i = 0; i < catalog.size; i++) {
        ResourceCatalog::Entry *walker = catalog.entries[i];

        printf("[%llu] o-> ", i);

        while(walker) {
            printf("%s o-> ", walker->key);
            walker = walker->next_in_hash;
        }

        printf("\n");
    }
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
            printf("Unsupported texture channels number\n");
        }

        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        printf("Error loading texture.\n");
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

internal u32 get_texture(const char *name) {
    u64 index = catalog_get(game_state.resources.texture_catalog, name);

    assert(index != UINT64_MAX);

    u32 texture = *game_state.resources.textures.get(index);

    return texture;
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

        if (walker[0] == '!') { // New material!

            // We already parsed a material, save it!
            if (material_name.length() != 0) {
                printf("Loaded material: %s\n", material_name.c_str());

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
        printf("Loaded material: %s\n", material_name.c_str());

        resources->materials.add(material);

        u64 material_index = resources->materials.length - 1;

        catalog_put(resources->material_catalog, material_name.c_str(), material_index);
    }
}

internal u64 get_material_index(const char *name) {
    u64 index = catalog_get(game_state.resources.material_catalog, name);

    assert(index != UINT64_MAX);

    return index;
}