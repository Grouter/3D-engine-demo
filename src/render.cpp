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

internal void _allocate_instance_buffer_particle(u32 *vao, u32 *instance_buffer, u64 size) {
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

        glBufferData(GL_ARRAY_BUFFER, size * sizeof(DrawCallDataParticle), nullptr, GL_STATIC_DRAW);

        i32 vector4_size = sizeof(Vector4);
        i32 stride = sizeof(DrawCallDataParticle);
        i32 attr_index = 2;
        u64 offset = 0;

        // TEXTURE
        offset += sizeof(i32); // The shader data will ignore this but offset must be set

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

internal void _allocate_instance_buffer_grass(u32 *vao, u32 *instance_buffer, u64 size) {
    Mesh *mesh = get_mesh("grass");

    glGenVertexArrays(1, vao);
    glBindVertexArray(*vao);

    {   // Index data
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
    }

    {   // Vertex data
        glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    }

    {   // UV data
        glBindBuffer(GL_ARRAY_BUFFER, mesh->nbo);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    }

    {   // UV data
        glBindBuffer(GL_ARRAY_BUFFER, mesh->tbo);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    }

    // Instance data
    {
        glGenBuffers(1, instance_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, *instance_buffer);

        glBufferData(GL_ARRAY_BUFFER, size * sizeof(Matrix4x4), nullptr, GL_STATIC_DRAW);

        i32 vector4_size = sizeof(Vector4);
        i32 stride = sizeof(Matrix4x4);
        i32 attr_index = 3;
        u64 offset = 0;

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
    // Basic 3D
    allocate_array(_draw_calls, MAX_DRAW_CALLS);

    // Grass
    allocate_array(_draw_calls_grass.data, MAX_GRASS_DRAW_CALLS);
    _allocate_instance_buffer_grass(&_draw_calls_grass.vao, &_draw_calls_grass.instance_buffer, MAX_GRASS_DRAW_CALLS);

    // Fonts
    _allocate_font_draw_call_buffer(_font_draw_calls[FontResource_Small],  MAX_DRAW_CALLS);
    _allocate_font_draw_call_buffer(_font_draw_calls[FontResource_Medium], MAX_DRAW_CALLS);
    _allocate_font_draw_call_buffer(_font_draw_calls[FontResource_Big],    MAX_DRAW_CALLS);

    // 2D
    allocate_array(_2d_shapes_draw_calls.data, MAX_DRAW_CALLS);
    _allocate_instance_buffer(&_2d_shapes_draw_calls.vao, &_2d_shapes_draw_calls.instance_buffer, MAX_DRAW_CALLS);

    // Particles
    allocate_array(_particle_draw_calls.data, MAX_PARTICLE_DRAW_CALLS);
    _allocate_instance_buffer_particle(&_particle_draw_calls.vao, &_particle_draw_calls.instance_buffer, MAX_PARTICLE_DRAW_CALLS);

    // Allocate HDR
    {
        u32 hdr;
        glGenFramebuffers(1, &hdr);
        glBindFramebuffer(GL_FRAMEBUFFER, hdr);

        u32 color_buffer;
        glGenTextures(1, &color_buffer);
        glBindTexture(GL_TEXTURE_2D, color_buffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, HDR_TARGET_W, HDR_TARGET_H, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_buffer, 0);

        u32 depth_buffer;
        glGenRenderbuffers(1, &depth_buffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, HDR_TARGET_W, HDR_TARGET_H);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buffer);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        game_state.hdr_framebuffer = hdr;
        game_state.post_color_buffer = color_buffer;
        game_state.post_depth_buffer = depth_buffer;
    }
}

internal void set_shader(ShaderResource shader) {
    Program *found = &game_state.resources.programs[shader];

    current_shader = found;

    glUseProgram(current_shader->handle);
}

internal void set_shader_vec3(const char *attr, Vector3 value) {
    i32 loc = glGetUniformLocation(current_shader->handle, attr);
    glUniform3f(loc, value.x, value.y, value.z);

#ifdef UNIFORM_DEBUG
    if (loc < 0) log_print("Shader set_shader_vec3 (%s) loc error!\n", attr);
#endif
}

internal void set_shader_vec4(const char *attr, Vector4 value) {
    i32 loc = glGetUniformLocation(current_shader->handle, attr);
    glUniform4f(loc, value.x, value.y, value.z, value.w);

#ifdef UNIFORM_DEBUG
    if (loc < 0) log_print("Shader set_shader_vec4 (%s) loc error!\n", attr);
#endif
}

internal void set_shader_matrix4x4(const char *attr, Matrix4x4 value) {
    i32 loc = glGetUniformLocation(current_shader->handle, attr);
    glUniformMatrix4fv(loc, 1, false, value.raw);

#ifdef UNIFORM_DEBUG
    if (loc < 0) log_print("Shader set_shader_matrix4x4 (%s) loc error!\n", attr);
#endif
}

internal void set_shader_matrix4x4_array(const char *attr, Matrix4x4 *values, u32 count) {
    i32 loc = glGetUniformLocation(current_shader->handle, attr);
    glUniformMatrix4fv(loc, count, false, (f32 *)values);

#ifdef UNIFORM_DEBUG
    if (loc < 0) log_print("Shader set_shader_matrix4x4_array (%s) loc error!\n", attr);
#endif
}

internal void set_shader_sampler(const char *attr, u32 texture_loc, u32 texture_handle) {
    glActiveTexture(GL_TEXTURE0 + texture_loc);
    glBindTexture(GL_TEXTURE_2D, texture_handle);

    i32 loc = glGetUniformLocation(current_shader->handle, attr);
    glUniform1i(loc, texture_loc);

#ifdef UNIFORM_DEBUG
    if (loc < 0) log_print("Shader set smapler (%s) %d loc error!\n", attr, texture_loc);
#endif
}

internal void set_shader_sampler_array(const char *attr, u32 texture_loc, u32 texture_handle) {
    glActiveTexture(GL_TEXTURE0 + texture_loc);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture_handle);

    i32 loc = glGetUniformLocation(current_shader->handle, attr);
    glUniform1i(loc, texture_loc);

#ifdef UNIFORM_DEBUG
    if (loc < 0) log_print("Shader set smapler array (%s) %d loc error!\n", attr, texture_loc);
#endif
}

internal void set_shader_int(const char *name, i32 value) {
    i32 loc = glGetUniformLocation(current_shader->handle, name);
    glUniform1i(loc, value);

#ifdef UNIFORM_DEBUG
    if (loc < 0) log_print("Shader set int loc error! (attribute: %s)\n", name);
#endif
}

internal void set_shader_float_array(const char *attr, f32 *values, u32 count) {
    i32 loc = glGetUniformLocation(current_shader->handle, attr);
    glUniform1fv(loc, count, values);

#ifdef UNIFORM_DEBUG
    if (loc < 0) log_print("Shader set_shader_float_array loc error! (attribute: %s)\n", attr);
#endif
}

internal void set_shader_float(const char *attr, f32 value) {
    i32 loc = glGetUniformLocation(current_shader->handle, attr);
    glUniform1f(loc, value);

#ifdef UNIFORM_DEBUG
    if (loc < 0) log_print("Shader set_shader_float loc error! (attribute: %s)\n", attr);
#endif
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

    if (!entity.program) {
        log_print("!!! Trying to render an entity with a NULL program\n");
        return;
    }

    for (u32 i = 0; i < mesh->sub_meshes.length; i++) {
        DrawCallData data;
        data.flags.raw = 0;

        data.flags.shader = (u32)(entity.program - game_state.resources.programs);

        if (data.flags.shader != 0) {
            int a = 0;
            a += 1;
        }

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

internal void draw_particle(Vector3 position, Vector2 size, u32 texture, Color color = Color_WHITE) {
    DrawCallDataParticle draw_call = {};
    {
        draw_call.texture = texture;
        draw_call.color = color_to_v4(color);

        draw_call.transform = look_at(position, game_state.camera.position);
        scale(draw_call.transform, size.x, size.y, 1.0f);
    }

    if (_particle_draw_calls.data.is_full()) {
        log_print("Exceeding particles draw calls capacity!!!\n");
    }

    _particle_draw_calls.data.add(draw_call);
}

internal int _draw_call_cmp(const void *a, const void *b) {
    DrawCallData *data1 = (DrawCallData *)a;
    DrawCallData *data2 = (DrawCallData *)b;

    if (data1->flags.raw < data2->flags.raw) return -1;
    if (data1->flags.raw > data2->flags.raw) return  1;
    return 0;
}

internal int _draw_particle_call_cmp(const void *a, const void *b) {
    DrawCallDataParticle *data1 = (DrawCallDataParticle *)a;
    DrawCallDataParticle *data2 = (DrawCallDataParticle *)b;

    if (data1->texture < data2->texture) return -1;
    if (data1->texture > data2->texture) return  1;
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

internal void flush_draw_calls_particles() {
    DrawCallBufferParticles &draw_calls = _particle_draw_calls;

    if (draw_calls.data.length == 0) return;

    qsort(_particle_draw_calls.data.data, _particle_draw_calls.data.length, sizeof(DrawCallDataParticle), _draw_particle_call_cmp);

    glNamedBufferSubData(draw_calls.instance_buffer, 0, draw_calls.data.length * sizeof(DrawCallDataParticle), draw_calls.data.data);

    glBindVertexArray(draw_calls.vao);

    u32 active_texture = UINT32_MAX;

    u32 start = 0;
    u32 end = 0;

    while (1) {
        if (active_texture != draw_calls.data[start].texture) {
            active_texture = draw_calls.data[start].texture;
            set_shader_sampler("diffuse_texture", 0, active_texture);
        }

        while (end < draw_calls.data.length && draw_calls.data[start].texture == draw_calls.data[end].texture) {
            end += 1;
        }

        u32 instances = end - start;
        glDrawElementsInstancedBaseInstance(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, instances, (i32)start);

        start = end;

        if (end >= draw_calls.data.length) break;
    }

    draw_calls.data.clear();
}

internal void flush_draw_calls_grass() {
    DrawCallBufferGrass &draw_calls = _draw_calls_grass;

    if (draw_calls.data.length == 0) return;

    set_shader(ShaderResource_Grass);
    set_shader_matrix4x4("view", game_state.camera.transform);
    set_shader_matrix4x4("projection", game_state.camera.perspective);
    set_shader_sampler_array("shadow_textures", 1, game_state.light_data.shadow_maps);
    set_shader_vec3("sun_dir", game_state.light_data.sun_direction);
    set_shader_float_array("cascade_distances", (game_state.light_data.cascade_splits + 1), SHADOW_CASCADE_COUNT);
    set_shader_float("time", game_state.time_elapsed);
    set_shader_int("point_light_count", (i32)game_state.light_data.point_lights.length);
    set_shader_int("unlit", 0);

    Material *material = &game_state.resources.materials[get_material_index("grass")];

    set_shader_vec4("material_color", material->color);
    glActiveTexture(GL_TEXTURE0);
    set_shader_sampler("diffuse_texture", 0, material->texture);

    glNamedBufferSubData(draw_calls.instance_buffer, 0, draw_calls.data.length * sizeof(Matrix4x4), draw_calls.data.data);

    glBindVertexArray(draw_calls.vao);

    Mesh *mesh = get_mesh("grass");

    glDrawElementsInstancedBaseInstance(GL_TRIANGLES, (i32)mesh->indicies.length, GL_UNSIGNED_INT, nullptr, (i32)draw_calls.data.length, 0);

    draw_calls.data.clear();
}

internal void flush_draw_calls() {
    // This will batch the draw calls
    qsort(_draw_calls.data, _draw_calls.length, sizeof(DrawCallData), _draw_call_cmp);

    u32 active_vao = UINT32_MAX;

    Material *material;
    u64 active_material_index = UINT64_MAX;

    u32 active_texture = UINT32_MAX;
    u32 active_shader = UINT32_MAX;

    DrawCallData *data;
    array_foreach(_draw_calls, data) {
        if (active_shader != data->flags.shader) {
            active_shader = data->flags.shader;
            set_shader((ShaderResource)active_shader);

            set_shader_matrix4x4("view", game_state.camera.transform);
            set_shader_matrix4x4("projection", game_state.camera.perspective);
            set_shader_sampler_array("shadow_textures", 1, game_state.light_data.shadow_maps);
            set_shader_vec3("sun_dir", game_state.light_data.sun_direction);
            set_shader_float_array("cascade_distances", (game_state.light_data.cascade_splits + 1), SHADOW_CASCADE_COUNT);
            set_shader_float("time", game_state.time_elapsed);
            set_shader_vec4("material_color", V4_ONE);
            set_shader_int("point_light_count", (i32)game_state.light_data.point_lights.length);
            set_shader_int("unlit", 0);
        }

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
                set_shader_int("unlit", material->unlit);

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