#if 0
internal void calc_shadowmap_split_distances(Camera &camera, LightData &light_data) {
    light_data.split_distances[0] = camera.clip_near;

    for (i32 i = 0; i < SHADOW_CASCADE_COUNT; i++) {
        f32 f = (f32)i / (f32)SHADOW_CASCADE_COUNT;
        f32 c_log = camera.clip_near * pow(camera.clip_far / camera.clip_near, f);
        f32 c_uni =  camera.clip_near + (camera.clip_far - camera.clip_near) * f;

        light_data.split_distances[i] = SHADOW_SPLIT_LAMBDA * c_log + (1.0f - SHADOW_SPLIT_LAMBDA) * c_uni;
    }

    light_data.split_distances[0] = camera.clip_near;
    light_data.split_distances[SHADOW_CASCADE_COUNT] = camera.clip_far;
}

internal void calc_shadowmap_cascade_projections(Camera &camera, LightData &light_data) {
    f32 fov_radians = TO_RADIANS(camera.fov);
    f32 tan_fov = tan(fov_radians * 0.5f);
    f32 aspect = VIRTUAL_WINDOW_W / VIRTUAL_WINDOW_H;

    Vector3 light_up = get_up_vector(light_data.sun_view);
    Vector3 light_right = get_side_vector(light_data.sun_view);

    for (i32 i = 0; i < SHADOW_CASCADE_COUNT; i++) {
        f32 h_near = 2.0f * tan_fov * light_data.split_distances[i];
        f32 w_near = h_near * aspect;

        f32 h_far = 2.0f * tan_fov * light_data.split_distances[i + 1];
        f32 w_far = h_far * aspect;

        Vector3 c_near = camera.position + (light_data.sun_direction * light_data.split_distances[i]);
        Vector3 c_far = camera.position + (light_data.sun_direction * light_data.split_distances[i + 1]);

        Vector3 corners[8];
        f32 lengths[8];

        // Calc corners in world space
        corners[0] = c_near + light_up * h_near * 0.5f - light_right * w_near * 0.5f;
        corners[1] = c_near + light_up * h_near * 0.5f + light_right * w_near * 0.5f;
        corners[2] = c_near - light_up * h_near * 0.5f - light_right * w_near * 0.5f;
        corners[3] = c_near - light_up * h_near * 0.5f + light_right * w_near * 0.5f;
        corners[4] = c_far + light_up * h_far * 0.5f - light_right * w_far * 0.5f;
        corners[5] = c_far + light_up * h_far * 0.5f + light_right * w_far * 0.5f;
        corners[6] = c_far - light_up * h_far * 0.5f - light_right * w_far * 0.5f;
        corners[7] = c_far - light_up * h_far * 0.5f + light_right * w_far * 0.5f;

        // Transform corners into the light space
        i32 min = 0;
        i32 max = 0;

        for (i32 j = 0; j < 8; j++) {
            corners[j] = multiply(light_data.sun_view, corners[j]);
            lengths[j] = length(corners[j]);

            if (lengths[j] > lengths[max]) max = j;
            if (lengths[j] < lengths[min]) min = j;
        }

        // Find the min and the max
        light_data.cascade_projections[i] = ortho(corners[min].z, corners[max].z, corners[min].x, corners[max].x, corners[max].y, corners[min].y);
    }
}
#endif