internal void allocate_resource_catalog(ResourceCatalog &catalog, u64 size) {
    catalog.size = size;
    catalog.entries = (ResourceCatalog::Entry **)calloc(size, sizeof(ResourceCatalog::Entry *));
}

internal u64 _catalog_hash(const char *key) {
    if (!key || !key[0]) return 1;

    return key[0] * 33 * _catalog_hash(key + 1);
}

internal void catalog_put(ResourceCatalog &catalog, const char *key, void *data_ptr) {
    u64 hashed_index = _catalog_hash(key) % catalog.size;
    u64 key_size = strlen(key);

    ResourceCatalog::Entry *entry = catalog.entries[hashed_index];

    if (entry == nullptr) {
        catalog.entries[hashed_index] = (ResourceCatalog::Entry *)malloc(sizeof(ResourceCatalog::Entry));

        entry = catalog.entries[hashed_index];

        entry->key = (char *)malloc((key_size + 1) * sizeof(char));
        strcpy_s(entry->key, key_size + 1, key);

        entry->data = data_ptr;
        entry->next_in_hash = nullptr;

        return;
    }

    while (1) {
        if (strcmp(entry->key, key) == 0) { // We just update the value.
            entry->data = data_ptr;
            return;
        }

        if (entry->next_in_hash == nullptr) {   // We found the last one!
            ResourceCatalog::Entry *new_entry = (ResourceCatalog::Entry *)malloc(sizeof(ResourceCatalog::Entry));

            new_entry->key = (char *)malloc((key_size + 1) * sizeof(char));
            strcpy_s(new_entry->key, key_size + 1, key);
            new_entry->data = data_ptr;
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

internal void *catalog_get(ResourceCatalog &catalog, const char *key) {
    ResourceCatalog::Entry *walker;

    for (u64 i = 0; i < catalog.size; i++) {
        walker = catalog.entries[i];

        while(walker) {
            if (strcmp(walker->key, key) == 0) {
                return walker->data;
            }

            walker = walker->next_in_hash;
        }
    }

    return nullptr;
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

internal u32 load_texture(char *image) {

    char path[] = "textures/";
    strcat(path, image);

    u32 texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);

    i32 width, height, nr_channels;
    u8 *data = stbi_load(path, &width, &height, &nr_channels, 0);

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