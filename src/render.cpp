internal void _allocate_font_instance_buffer(u32 *vao, u32 *instance_buffer, u64 size) {
    Mesh *quad = &game_state.resources.meshes[MeshResource_Quad];

    glGenVertexArrays(1, vao);
    glBindVertexArray(*vao);

    {   // Index data
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad->ebo);
    }

    {   // Vertex data
        glBindBuffer(GL_ARRAY_BUFFER, quad->vbo);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    }

    {   // UV data
        glBindBuffer(GL_ARRAY_BUFFER, quad->tbo);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    }

    // Instance data
    {
        glGenBuffers(1, instance_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, *instance_buffer);

        glBufferData(GL_ARRAY_BUFFER, size * sizeof(FontDrawCallData), nullptr, GL_STATIC_DRAW);

        i32 vector4_size = sizeof(Vector4);
        i32 stride = sizeof(FontDrawCallData);
        i32 attr_index = 2;
        u64 offset = 0;

        // COLOR
        glEnableVertexAttribArray(attr_index);
        glVertexAttribPointer(attr_index, 4, GL_FLOAT, GL_FALSE, stride, (void *)offset);
        glVertexAttribDivisor(attr_index, 1);
        offset += vector4_size;
        attr_index += 1;

        // UV OFFSET
        glEnableVertexAttribArray(attr_index);
        glVertexAttribPointer(attr_index, 2, GL_FLOAT, GL_FALSE, stride, (void *)offset);
        glVertexAttribDivisor(attr_index, 1);
        offset += sizeof(Vector2);
        attr_index += 1;

        // UV SCALE
        glEnableVertexAttribArray(attr_index);
        glVertexAttribPointer(attr_index, 2, GL_FLOAT, GL_FALSE, stride, (void *)offset);
        glVertexAttribDivisor(attr_index, 1);
        offset += sizeof(Vector2);
        attr_index += 1;

        // MATRIX 0
        glEnableVertexAttribArray(attr_index);
        glVertexAttribPointer(attr_index, 4, GL_FLOAT, GL_FALSE, stride, (void *)offset);
        glVertexAttribDivisor(attr_index, 1);
        offset += vector4_size;
        attr_index += 1;

        // MATRIX 1
        glEnableVertexAttribArray(attr_index);
        glVertexAttribPointer(attr_index, 4, GL_FLOAT, GL_FALSE, stride, (void *)offset);
        glVertexAttribDivisor(attr_index, 1);
        offset += vector4_size;
        attr_index += 1;

        // MATRIX 2
        glEnableVertexAttribArray(attr_index);
        glVertexAttribPointer(attr_index, 4, GL_FLOAT, GL_FALSE, stride, (void *)offset);
        glVertexAttribDivisor(attr_index, 1);
        offset += vector4_size;
        attr_index += 1;

        // MATRIX 3
        glEnableVertexAttribArray(attr_index);
        glVertexAttribPointer(attr_index, 4, GL_FLOAT, GL_FALSE, stride, (void *)offset);
        glVertexAttribDivisor(attr_index, 1);
        offset += vector4_size;
        attr_index += 1;
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

internal void _allocate_font_draw_call_buffer(FontDrawCallBuffer &buffer, u64 size) {
    allocate_array(buffer.data, size);
    _allocate_font_instance_buffer(&buffer.vao, &buffer.instance_buffer, size);
}

internal void init_renderer() {
    allocate_array(_draw_calls, MAX_DRAW_CALLS);

    _allocate_font_draw_call_buffer(_font_draw_calls[FontResource_Small],  MAX_DRAW_CALLS);
    _allocate_font_draw_call_buffer(_font_draw_calls[FontResource_Medium], MAX_DRAW_CALLS);
    _allocate_font_draw_call_buffer(_font_draw_calls[FontResource_Big],    MAX_DRAW_CALLS);
}

internal void set_shader(ShaderResource shader) {
    Program *found = &game_state.resources.programs[shader];

    current_shader = found;

    glUseProgram(current_shader->handle);
}

internal void set_shader_projection(Matrix4x4 perspective) {
    i32 loc = glGetUniformLocation(current_shader->handle, "projection");

    if (loc >= 0) {
        glUniformMatrix4fv(loc, 1, false, perspective.raw);
    }
    else {
        log_print("Shader perspective loc error!\n");
    }
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
        glUniformMatrix4fv(loc, 1, GL_FALSE, view.raw);
    }
    else {
        log_print("Shader set view loc error!\n");
    }
}

internal void set_shader_material_color(Vector4 color) {
    i32 loc = glGetUniformLocation(current_shader->handle, "material_color");

    if (loc >= 0) {
        glUniform4f(loc, color.r, color.g, color.b, color.a);
    }
    else {
        log_print("Shader set material_color loc error!\n");
    }
}

// Position = bottom left of the rendered text bounding box
internal void draw_text(const Font &font, char *text, Vector3 position, Color color, f32 size_scale = 1.0f) {
    while (*text) {
        GlyphRenderData glyph = get_glyph(font, *text);

        if (*text == ' ') {
            // If space, just advance to the next character
            position.x += glyph.x_advance * size_scale * game_state.pixels_to_units;
            text += 1;
            continue;
        }

        FontDrawCallData draw_call = {};
        {
            draw_call.color = color_to_v4(color);
            draw_call.transform = identity();

            f32 x_offset = (glyph.world_scale.x * size_scale * 0.5f) + (glyph.offset.x * size_scale * game_state.pixels_to_units);
            f32 y_offset = (glyph.world_scale.y * size_scale * 0.5f) - (glyph.offset.y * size_scale * game_state.pixels_to_units);

            translate(draw_call.transform, position.x + x_offset, position.y + y_offset, position.z);

            scale(draw_call.transform, glyph.world_scale.x * size_scale, glyph.world_scale.y * size_scale, 1.0f);

            draw_call.uv_offset = glyph.uv_offset;
            draw_call.uv_scale = glyph.uv_scale;
        }

        if (_font_draw_calls[font.resource_id].data.is_full()) {
            log_print("Exceeding font draw calls capacity!!!\n");
        }

        _font_draw_calls[font.resource_id].data.add(draw_call);

        // Advance to the next character
        position.x += glyph.x_advance * size_scale * game_state.pixels_to_units;
        text += 1;
    }
}

internal void render_entity(Entity &entity) {
    Mesh *mesh = entity.mesh;

    if (mesh == nullptr || mesh->loaded == false) {
        return;
    }

    Matrix4x4 transform = identity();
    translate(transform, entity.position.x, entity.position.y, entity.position.z);

    // @Todo: quaternions here!
    rotate(transform, entity.rotation.x, entity.rotation.y, entity.rotation.z);

    for (u32 i = 0; i < mesh->sub_meshes.length; i++) {
        DrawCallData data;
        data.flags = 0;

        // @Todo: set shader to flags

        if (mesh->sub_meshes.data[i].material_index >= game_state.resources.materials.length) {
            data.flags |= (0 << RenderDataFlagBits::ShaderBits);
        }
        else {
            data.flags |= (mesh->sub_meshes.data[i].material_index << RenderDataFlagBits::ShaderBits);
        }

        data.info = mesh->sub_meshes.data[i];
        data.mesh = mesh;
        data.transform = transform;

        _draw_calls.add(data);
    }
}

internal int _draw_call_cmp(const void *a, const void *b) {
    DrawCallData *data1 = (DrawCallData *)a;
    DrawCallData *data2 = (DrawCallData *)b;

    if (data1->flags < data2->flags) return -1;
    if (data1->flags > data2->flags) return  1;
    return 0;
}

internal void flush_font_draw_calls() {
    set_shader(ShaderResource::ShaderResource_Font);
    set_shader_projection(game_state.font_proj);

    for (u32 i = 0; i < FontResource_COUNT; i++) {
        FontDrawCallBuffer &draw_calls = _font_draw_calls[i];

        if (draw_calls.data.length == 0) continue;

        Font &font = game_state.resources.fonts[i];

        // Upload new data to the instance buffer
        glNamedBufferSubData(draw_calls.instance_buffer, 0, draw_calls.data.length * sizeof(FontDrawCallData), draw_calls.data.data);

        glBindVertexArray(draw_calls.vao);

        set_shader_diffuse_texture(font.texture);

        glDrawElementsInstancedBaseInstance(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, (i32)draw_calls.data.length, 0);

        draw_calls.data.clear();
    }
}

internal void flush_draw_calls() {
    set_shader(ShaderResource::ShaderResource_Default);
    set_shader_view(game_state.camera.transform);
    set_shader_projection(game_state.camera.perspective);

    u32 active_vao = UINT32_MAX;

    Material *material;
    u64 active_material_index = UINT64_MAX;

    u32 active_texture = UINT32_MAX;

    // This will batch the draw calls
    qsort(_draw_calls.data, _draw_calls.length, sizeof(DrawCallData), _draw_call_cmp);

    DrawCallData *data;
    array_foreach(_draw_calls, data) {
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
                material = &game_state.resources.materials[material_index];

                set_shader_material_color(material->color);

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

        u64 offset = data->info.first_index * sizeof(u32);
        glDrawElements(GL_TRIANGLES, (i32)data->info.index_count, GL_UNSIGNED_INT, (void *)offset);
    }

    _draw_calls.clear();
}