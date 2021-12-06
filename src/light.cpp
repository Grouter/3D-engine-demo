internal void init_light_buffers(LightData &data) {
    u32 shader_handle = game_state.resources.programs[ShaderResource_Shadow].handle;

    u32 lights_buffer_index = glGetUniformBlockIndex(shader_handle, "LightMatricies");

    glGenBuffers(1, &data.shadow_uniform_buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, data.shadow_uniform_buffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(Matrix4x4) * SHADOW_CASCADE_COUNT, nullptr, GL_DYNAMIC_DRAW);
    glUniformBlockBinding(shader_handle, lights_buffer_index, 0);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, data.shadow_uniform_buffer, 0, sizeof(Matrix4x4) * SHADOW_CASCADE_COUNT);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

internal void calc_shadowmap_split_distances(Camera &camera, LightData &light_data) {
    for (i32 i = 0; i < SHADOW_CASCADE_COUNT; i++) {
        f32 f = (f32)i / (f32)SHADOW_CASCADE_COUNT;
        f32 c_log = camera.clip_near * pow(camera.clip_far / camera.clip_near, f);
        f32 c_uni = camera.clip_near + (camera.clip_far - camera.clip_near) * f;

        light_data.cascade_splits[i] = SHADOW_SPLIT_LAMBDA * c_log + (1.0f - SHADOW_SPLIT_LAMBDA) * c_uni;
    }

    light_data.cascade_splits[0] = camera.clip_near;
    light_data.cascade_splits[SHADOW_CASCADE_COUNT] = camera.clip_far;
}

internal void calc_cascade_matricies(Camera &camera, LightData &light_data) {
    f32 fov_radians = TO_RADIANS(camera.fov * 0.5f);
    f32 tan_fov = tan(fov_radians);
    f32 aspect = (f32)VIRTUAL_WINDOW_W / (f32)VIRTUAL_WINDOW_H;

    Vector3 light_right = normalized(cross(V3_UP, light_data.sun_direction));
    Vector3 light_up = normalized(cross(light_data.sun_direction, light_right));

    Vector3 corners[8];

    for (i32 i = 0; i < SHADOW_CASCADE_COUNT; i++) {
        f32 h_near = 2.0f * tan_fov * light_data.cascade_splits[i];
        f32 w_near = h_near * aspect;

        f32 h_far = 2.0f * tan_fov * light_data.cascade_splits[i + 1];
        f32 w_far = h_far * aspect;

        Vector3 c_near = camera.position + (camera.direction * light_data.cascade_splits[i]);
        Vector3 c_far = camera.position + (camera.direction * light_data.cascade_splits[i + 1]);

        // Calc corners in world space
        corners[0] = c_near + light_up * h_near * 0.5f - light_right * w_near * 0.5f;
        corners[1] = c_near + light_up * h_near * 0.5f + light_right * w_near * 0.5f;
        corners[2] = c_near - light_up * h_near * 0.5f - light_right * w_near * 0.5f;
        corners[3] = c_near - light_up * h_near * 0.5f + light_right * w_near * 0.5f;

        corners[4] = c_far + light_up * h_far * 0.5f - light_right * w_far * 0.5f;
        corners[5] = c_far + light_up * h_far * 0.5f + light_right * w_far * 0.5f;
        corners[6] = c_far - light_up * h_far * 0.5f - light_right * w_far * 0.5f;
        corners[7] = c_far - light_up * h_far * 0.5f + light_right * w_far * 0.5f;

        Vector3 center = {};
        for (i32 j = 0; j < 8; j++) center += corners[j];
        center /= 8.0f;

        Matrix4x4 light_view = look_at(center, center + light_data.sun_direction);

        f32 radius = -FLT_MAX;
        for (i32 j = 0; j < 8; j++) {
            f32 d = distance(center, corners[j]);
            radius = max(radius, d);
        }

        Vector3 max_coords = make_vector3(radius);
        Vector3 min_coords = make_vector3(-radius);

        light_data.cascade_projections[i] = ortho(min_coords.z, max_coords.z, min_coords.x, max_coords.x, max_coords.y, min_coords.y);
        light_data.cascade_mvps[i] = multiply(light_data.cascade_projections[i], light_view);
    }
}