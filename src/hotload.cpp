global std::mutex _hotload_mutex;

global Array<HotloadShaderEntry> _hotload_shader_queue;
global bool _hotload_resource_queue[HotloadResource_COUNT];

internal void init_hotload() {
    allocate_array(_hotload_shader_queue, 10);
}

internal void hotload_watcher() {
    log_print("Hotload started!\n");

    HANDLE watch_handle = CreateFile(
        "./",   // the asset folder
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_DELETE | FILE_SHARE_WRITE,
        nullptr,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        nullptr
    );

    u8 *buffer = (u8 *)calloc(2048, 1);
    char file_path[2048];

    DWORD b_returned;

    while (1) {
        bool result = ReadDirectoryChangesW(
            watch_handle,
            buffer,
            2048,
            true,
            FILE_NOTIFY_CHANGE_LAST_WRITE,
            &b_returned,
            nullptr,
            nullptr
        );

        if (result == 0) {
            log_print("Read dir changes failed err code: %d\n", GetLastError());
            break;
        }

        FILE_NOTIFY_INFORMATION *info = (FILE_NOTIFY_INFORMATION *)buffer;

        if (info->Action != FILE_ACTION_MODIFIED) continue;

        u64 file_path_size = info->FileNameLength / sizeof(wchar_t);

        // Reset the filename buffer and convert wide chars to normal chars
        // while writing the the filename buffer
        memset(file_path, 0, 2048);
        for (u64 i = 0; i < file_path_size; i++) {
            file_path[i] = (char)info->FileName[i];
        }

        log_print("Hotload trigger: %s\n", file_path);

        char *file_name = file_path + file_path_size;
        while (file_name != file_path) {
            if (*file_name == '\\' || *file_name == '/') {
                file_name += 1;
                break;
            }
            file_name -= 1;
        }

        u64 file_name_size = file_path_size - (file_name - file_path);

        // File extension
        char *extension = file_path;
        extension = eat_until(extension, '.');
        extension += 1;

        if (strncmp(extension, "glsl", 4) == 0) {
            // @Todo: I don't like this for loop check
            bool contains = false;
            for (u64 i = 0; i < _hotload_shader_queue.length; i++) {
                char *name = _hotload_shader_queue.data[i].shader_name;

                if (strncmp(name, file_name, file_name_size) == 0) {
                    contains = true;
                    break;
                }
            }

            if (!contains) {
                _hotload_mutex.lock();

                HotloadShaderEntry shader_queue_entry = {};
                strncpy(shader_queue_entry.shader_name, file_name, file_name_size);
                _hotload_shader_queue.add(shader_queue_entry);

                _hotload_mutex.unlock();
            }
        }
        else if (strncmp(extension, "resources", 9) == 0) {
            if (strncmp(file_name, "mesh", 4) == 0) {
                _hotload_mutex.lock();

                _hotload_resource_queue[HotloadResource_MESHES] = true;

                _hotload_mutex.unlock();

            }
            else if (strncmp(file_name, "material", 8) == 0) {
                _hotload_mutex.lock();

                _hotload_resource_queue[HotloadResource_MATERIALS] = true;

                _hotload_mutex.unlock();
            }
        }
        else if (strncmp(extension, "world", 4) == 0) {
            _hotload_mutex.lock();

            _hotload_resource_queue[HotloadResource_WORLD] = true;

            _hotload_mutex.unlock();
        }
    }

    free(buffer);
}

internal void process_hotload_queue(Resources &resources) {
    _hotload_mutex.lock();

    for (i64 i = _hotload_shader_queue.length - 1; i >= 0; i--) {
        char *shader_name = _hotload_shader_queue.data[i].shader_name;

        char path[64];
        snprintf(path, 64, "shaders/%s", shader_name);

        u64 shader_hash = _catalog_hash(path);

        for (u32 j = 0; j < ShaderResource_COUNT; j++) {
            if (resources.programs[j].source_file_hash == shader_hash) {
                Program &program = resources.programs[j];

                log_print("Reloding shader: %s (old handle: %u)\n", shader_name, program.handle);

                Program reloaded_shader = {};
                bool success = load_program(path, reloaded_shader, program.has_geometry, (u32)program.defines.length, program.defines.data);

                if (success) {
                    glUseProgram(0);

                    glDeleteProgram(program.handle);
                    program.handle = reloaded_shader.handle;

                    free_array(reloaded_shader.defines);
                }
            }
        }

        _hotload_shader_queue.remove_last();
    }

    if (_hotload_resource_queue[HotloadResource_MESHES]) {
        log_print("Reloading mesh resources!\n");

        unload_meshes();
        load_mesh_file();

        _hotload_resource_queue[HotloadResource_MESHES] = false;
    }

    if (_hotload_resource_queue[HotloadResource_MATERIALS]) {
        log_print("Reloading material resources!\n");

        unload_textures();
        unload_materials();

        load_material_file();

        _hotload_resource_queue[HotloadResource_MATERIALS] = false;
    }

    if (_hotload_resource_queue[HotloadResource_WORLD]) {
        log_print("Reloading world file!\n");

        Entity *it;
        BucketLocation loc;
        bucket_array_foreach_w_loc(game_state.entities.base_entities, it, loc) {
            if (it->type == EntityType_FLYING_ROCK) continue;

            it->flags.destroy = true;
        }}

        remove_flagged_entities(game_state.entities);

        game_state.light_data.point_lights.clear();

        load_world_file(game_state.entities);

        _hotload_resource_queue[HotloadResource_WORLD] = false;
    }

    _hotload_mutex.unlock();
}