internal void init_renderer() {
    allocate_array(draw_calls, 1024);
}

internal void set_shader(ShaderResource shader) {
    Program *found = &game_state.resources.programs[shader];

    current_shader = found;

    glUseProgram(current_shader->handle);
}

internal void set_shader_diffuse_texture(u32 texture_handle) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_handle);

    i32 loc = glGetUniformLocation(current_shader->handle, "diffuse_texture");

    if (loc >= 0) {
        glUniform1i(loc, 0);
    }
    else {
        log_print("Shader set diffuse texture loc error!\n");
    }
}

internal void set_shader_view(Matrix4x4 view) {
    i32 loc = glGetUniformLocation(current_shader->handle, "view");

    if (loc >= 0) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, game_state.camera.transform.raw);
    }
    else {
        log_print("Shader set view loc error!\n");
    }
}

internal void set_material_color(Vector3 color) {
    i32 loc = glGetUniformLocation(current_shader->handle, "material_color");

    if (loc >= 0) {
        glUniform3f(loc, color.r, color.g, color.b);
    }
    else {
        log_print("Shader set material_color loc error!\n");
    }
}

internal void render_entity(Entity &entity) {
    Mesh *mesh = entity.mesh;
    if (mesh == nullptr) {
        return;
    }

    Matrix4x4 transform = identity();
    translate(transform, entity.position.x, entity.position.y, entity.position.z);

    // @Todo: quaternions here!
    rotate(transform, entity.rotation.x, entity.rotation.y, entity.rotation.z);

    for (u32 i = 0; i < mesh->sub_meshes.length; i++) {
        DrawCallData data;
        data.flags = 0;

        {
            // @Todo: set shader to flags
            data.flags |= (entity.material_index << RenderDataFlagBits::ShaderBits);
        }

        data.info = mesh->sub_meshes.data[i];
        data.mesh = mesh;
        data.transform = transform;

        draw_calls.add(data);
    }
}

internal int _draw_call_cmp(const void *a, const void *b) {
    DrawCallData *data1 = (DrawCallData *)a;
    DrawCallData *data2 = (DrawCallData *)b;

    if (data1->flags < data2->flags) return -1;
    if (data1->flags > data2->flags) return  1;
    return 0;
}

internal void flush_draw_calls() {
    u32 active_vao = UINT32_MAX;

    Material *material;
    u64 active_material_index = UINT64_MAX;

    u32 active_texture = UINT32_MAX;

    // This will batch the draw calls
    qsort(draw_calls.data, draw_calls.length, sizeof(DrawCallData), _draw_call_cmp);

    DrawCallData *data;
    array_foreach(draw_calls, data) {
        // VAO switching
        if (active_vao != data->mesh->vao) {
            active_vao = data->mesh->vao;
            glBindVertexArray(data->mesh->vao);
        }

        // Material switching
        {
            u64 material_index = (u64)(data->flags >> RenderDataFlagBits::ShaderBits);
            material_index &= ~(UINT64_MAX << RenderDataFlagBits::MaterialBits);

            if (active_material_index != material_index) {
                active_material_index = material_index;
                material = game_state.resources.materials.get(material_index);

                set_material_color(material->color);

                if (active_texture != material->texture) {
                    set_shader_diffuse_texture(material->texture);
                }
            }
        }

        // Model transform upload
        {
            i32 loc = glGetUniformLocation(current_shader->handle, "model");
            glUniformMatrix4fv(loc, 1, GL_FALSE, data->transform.raw);
        }

        u64 offset = data->info.first_index * data->info.index_count;
        glDrawElements(GL_TRIANGLES, (i32)data->info.index_count, GL_UNSIGNED_INT, (const void *)offset);
    }

    draw_calls.clear();
}