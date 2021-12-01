internal void _allocate_instance_buffer(u32 *vao, u32 *instance_buffer, u64 size) {
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

        glBufferData(GL_ARRAY_BUFFER, size * sizeof(DrawCallData2D), nullptr, GL_STATIC_DRAW);

        i32 vector4_size = sizeof(Vector4);
        i32 stride = sizeof(DrawCallData2D);
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

internal void _allocate_font_draw_call_buffer(DrawCallBuffer2D &buffer, u64 size) {
    allocate_array(buffer.data, size);
    _allocate_instance_buffer(&buffer.vao, &buffer.instance_buffer, size);
}

internal void init_renderer() {
    allocate_array(_draw_calls, MAX_DRAW_CALLS);

    _allocate_font_draw_call_buffer(_font_draw_calls[FontResource_Small],  MAX_DRAW_CALLS);
    _allocate_font_draw_call_buffer(_font_draw_calls[FontResource_Medium], MAX_DRAW_CALLS);
    _allocate_font_draw_call_buffer(_font_draw_calls[FontResource_Big],    MAX_DRAW_CALLS);

    allocate_array(_2d_shapes_draw_calls.data, MAX_DRAW_CALLS);
    _allocate_instance_buffer(&_2d_shapes_draw_calls.vao, &_2d_shapes_draw_calls.instance_buffer, MAX_DRAW_CALLS);
}

internal void set_shader(ShaderResource shader) {
    Program *found = &game_state.resources.programs[shader];

    current_shader = found;

    glUseProgram(current_shader->handle);
}

internal void set_shader_vec3(const char *attr, Vector3 value) {
    i32 loc = glGetUniformLocation(current_shader->handle, attr);

    if (loc >= 0) {
        glUniform3f(loc, value.x, value.y, value.z);
    }
    else {
        log_print("Shader set_shader_vec3 (%s) loc error!\n", attr);
    }
}

internal void set_shader_vec4(const char *attr, Vector4 value) {
    i32 loc = glGetUniformLocation(current_shader->handle, attr);

    if (loc >= 0) {
        glUniform4f(loc, value.x, value.y, value.z, value.w);
    }
    else {
        log_print("Shader set_shader_vec4 (%s) loc error!\n", attr);
    }
}

internal void set_shader_matrix4x4(const char *attr, Matrix4x4 value) {
    i32 loc = glGetUniformLocation(current_shader->handle, attr);

    if (loc >= 0) {
        glUniformMatrix4fv(loc, 1, false, value.raw);
    }
    else {
        log_print("Shader set_shader_matrix4x4 (%s) loc error!\n", attr);
    }
}

internal void set_shader_sampler(const char *attr, u32 texture_loc, u32 texture_handle) {
    glActiveTexture(GL_TEXTURE0 + texture_loc);
    glBindTexture(GL_TEXTURE_2D, texture_handle);

    i32 loc = glGetUniformLocation(current_shader->handle, attr);

    if (loc >= 0) {
        glUniform1i(loc, texture_loc);
    }
    else {
        log_print("Shader set smapler (%s) %d loc error!\n", attr, texture_loc);
    }
}

internal void set_shader_int(const char *name, i32 value) {
    i32 loc = glGetUniformLocation(current_shader->handle, name);

    if (loc >= 0) {
        glUniform1i(loc, value);
    }
    else {
        log_print("Shader set int loc error! (attribute: %s)\n", name);
    }
}

// Position = bottom left of the rendered text bounding box
internal void draw_text(const Font &font, char *text, Vector3 position, Color color, f32 size_scale = 1.0f) {
    while (*text) {
        GlyphRenderData glyph = get_glyph(font, *text);

        if (*text == ' ') {
            // If space, just advance to the next character
            position.x += glyph.x_advance * size_scale * game_state.pixels_to_units_2d;
            text += 1;
            continue;
        }

        DrawCallData2D draw_call = {};
        {
            draw_call.color = color_to_v4(color);
            draw_call.transform = identity();

            f32 x_offset = (glyph.world_scale.x * size_scale * 0.5f) + (glyph.offset.x * size_scale * game_state.pixels_to_units_2d);
            f32 y_offset = (glyph.world_scale.y * size_scale * 0.5f) - (glyph.offset.y * size_scale * game_state.pixels_to_units_2d);

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
        position.x += glyph.x_advance * size_scale * game_state.pixels_to_units_2d;
        text += 1;
    }
}

internal void draw_rect(Vector3 position, Vector2 size, Color color) {
    DrawCallData2D draw_call = {};
    {
        draw_call.color = color_to_v4(color);

        draw_call.transform = identity();
        translate(draw_call.transform, position.x, position.y, position.z);
        scale(draw_call.transform, size.x, size.y, 1.0f);

        draw_call.uv_offset = {};
        draw_call.uv_scale  = V2_ONE;
    }

    if (_2d_shapes_draw_calls.data.is_full()) {
        log_print("Exceeding 2D shapes draw calls capacity!!!\n");
    }

    _2d_shapes_draw_calls.data.add(draw_call);
}

internal void render_entity(Entity &entity, Matrix4x4 transform) {
    Mesh *mesh = entity.mesh;

    if (mesh == nullptr || mesh->loaded == false) {
        return;
    }

    for (u32 i = 0; i < mesh->sub_meshes.length; i++) {
        DrawCallData data;
        data.flags.raw = 0;

        // @Todo: set shader to flags

        data.flags.shader = 0;

        if (mesh->sub_meshes.data[i].material_index < game_state.resources.materials.length) {
            data.flags.material = (u32)mesh->sub_meshes[i].material_index;
        }
        else data.flags.material = 0;

        data.info = mesh->sub_meshes.data[i];
        data.mesh = mesh;
        data.transform = transform;

        _draw_calls.add(data);
    }
}

internal int _draw_call_cmp(const void *a, const void *b) {
    DrawCallData *data1 = (DrawCallData *)a;
    DrawCallData *data2 = (DrawCallData *)b;

    if (data1->flags.raw < data2->flags.raw) return -1;
    if (data1->flags.raw > data2->flags.raw) return  1;
    return 0;
}

internal void flush_font_draw_calls() {
    for (u32 i = 0; i < FontResource_COUNT; i++) {
        DrawCallBuffer2D &draw_calls = _font_draw_calls[i];

        if (draw_calls.data.length == 0) continue;

        Font &font = game_state.resources.fonts[i];

        // Upload new data to the instance buffer
        glNamedBufferSubData(draw_calls.instance_buffer, 0, draw_calls.data.length * sizeof(DrawCallData2D), draw_calls.data.data);

        glBindVertexArray(draw_calls.vao);

        set_shader_sampler("diffuse_texture", 0, font.texture);

        glDrawElementsInstancedBaseInstance(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, (i32)draw_calls.data.length, 0);

        draw_calls.data.clear();
    }
}

internal void flush_2d_shapes_draw_calls() {
    DrawCallBuffer2D &draw_calls = _2d_shapes_draw_calls;

    if (draw_calls.data.length == 0) return;

    // Upload new data to the instance buffer
    glNamedBufferSubData(draw_calls.instance_buffer, 0, draw_calls.data.length * sizeof(DrawCallData2D), draw_calls.data.data);

    glBindVertexArray(draw_calls.vao);

    // White texture
    glActiveTexture(GL_TEXTURE0);
    set_shader_sampler("diffuse_texture", 0, game_state.resources.textures[0]);

    glDrawElementsInstancedBaseInstance(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, (i32)draw_calls.data.length, 0);

    draw_calls.data.clear();
}

internal void flush_draw_calls_shadow() {
    // This will batch the draw calls
    qsort(_draw_calls.data, _draw_calls.length, sizeof(DrawCallData), _draw_call_cmp);

    u32 active_vao = UINT32_MAX;

    DrawCallData *data;
    array_foreach(_draw_calls, data) {
        // VAO switching
        if (active_vao != data->mesh->vao) {
            active_vao = data->mesh->vao;
            glBindVertexArray(data->mesh->vao);
        }

        // Model transform upload
        set_shader_matrix4x4("model", data->transform);

        u64 offset = data->info.first_index * sizeof(u32);
        glDrawElements(GL_TRIANGLES, (i32)data->info.index_count, GL_UNSIGNED_INT, (void *)offset);
    }
}

internal void flush_draw_calls() {
    // This will batch the draw calls
    qsort(_draw_calls.data, _draw_calls.length, sizeof(DrawCallData), _draw_call_cmp);

    u32 active_vao = UINT32_MAX;

    Material *material;
    u64 active_material_index = UINT64_MAX;

    u32 active_texture = UINT32_MAX;

    DrawCallData *data;
    array_foreach(_draw_calls, data) {
        // VAO switching
        if (active_vao != data->mesh->vao) {
            active_vao = data->mesh->vao;
            glBindVertexArray(data->mesh->vao);
        }

        // Material switching
        {
            u64 material_index = data->flags.material;

            if (active_material_index != material_index) {
                active_material_index = material_index;
                material = &game_state.resources.materials[material_index];

                set_shader_vec4("material_color", material->color);

                if (active_texture != material->texture) {
                    glActiveTexture(GL_TEXTURE0);
                    set_shader_sampler("diffuse_texture", 0, material->texture);
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