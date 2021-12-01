internal void init_game() {
    srand((u32)time(0));

    game_state.time_elapsed = 0.0f;

    game_state.camera = create_camera(VIRTUAL_WINDOW_W, VIRTUAL_WINDOW_H, 80.0f);
    game_state.camera.rotation.x = 25.0f;
    game_state.camera.rotation.y = 90.0f;

    game_state.camera.position.y = 10.0f;
    game_state.camera.position.z = 10.0f;

    game_state.ortho_proj = ortho(-1.0f, 1.0f, VIRTUAL_WINDOW_W_2D, VIRTUAL_WINDOW_H_2D);

    // Init and load resources
    init_resources(game_state.resources);
    load_material_file();
    load_mesh_file();
    allocate_entity_storage(game_state.entities);
    load_world_file(game_state.entities);

    // Light
    init_light_data(game_state.light_data);
    game_state.light_data.sun_direction = normalized(make_vector3(0.2f, -1.0f, -0.3f));

    // Spawn rocks
    {
        f32 a_part = TWO_PI / ROCKS;
        f32 current_a = 0.0f;

        for (i32 i = 0; i < ROCKS; i++) {
            Entity *root = create_rock_formation(game_state.entities);

            f32 radius = rand_f_range(100, 200);

            root->position.x = sinf(current_a) * radius;
            root->position.y = rand_f_range(-50, 150);
            root->position.z = cosf(current_a) * radius;
            root->scale = make_vector3(8.0f);

            current_a += a_part;
        }
    }
}

internal void tick(f32 dt) {
    camera_handle_input(game_state.camera);

    camera_animate(game_state.camera, game_state.resources.camera_animation, dt);
    camera_update(game_state.camera);

    // Update entities
    {
        Entity *it;

        bucket_array_foreach(game_state.entities.base_entities, it) {
            if (it->type == EntityType_BIRD) {
                BirdData *bird_data = &game_state.entities.entity_data[it->data].bird_data;

                bird_data->hover_animation += dt * BIRD_HOVER_SPEED;
                it->position.y = BIRD_HEIGHTS + (sinf(bird_data->hover_animation) * BIRD_HOVER_AMPL);

                // Find new target if current is reached
                {
                    f32 d = distance(it->position, bird_data->fly_target);
                    if (d <= 1.0f) {
                        bird_data->fly_target.x = (f32)(rand() % 50) - 25.0f;
                        bird_data->fly_target.z = (f32)(rand() % 50) - 25.0f;
                    }
                }

                // Move towads target
                // @Todo: finer rotations
                {
                    Vector3 direction = bird_data->fly_target - it->position;
                    normalize(direction);

                    // @Todo: rotate to direction

                    direction *= BIRD_SPEED * dt;

                    it->position += direction;
                }
            }
            else if (it->type == EntityType_FLYING_ROCK) {
                FlyingRockData *rock_data = &game_state.entities.entity_data[it->data].flying_rock_data;

                it->rotation.y += dt * 0.01f * rock_data->rotation_direction;

                it->position.y += sinf(game_state.time_elapsed * 0.2f) * 0.001f * rock_data->rotation_direction;
            }
        }}
    }

    // Calculate sun transform for shadow calulations
    {
        Matrix4x4 sun_view = from_direction(game_state.light_data.sun_direction);
        game_state.light_data.sun_mvp = multiply(game_state.light_data.sun_projection, sun_view);
    }

    // Calculate transforms
    {
        FlyingRockTransformHierarchy &hierarchy = game_state.entities.flying_rock_transforms;

        hierarchy.local.clear();
        hierarchy.lookups.clear();
        hierarchy.results.clear();

        u32 last_root = 0;
        u32 last_sub = 0;

        Entity *it;
        bucket_array_foreach(game_state.entities.base_entities, it) {
            if (it->type == EntityType_FLYING_ROCK) {
                FlyingRockData *data = &game_state.entities.entity_data[it->data].flying_rock_data;

                Matrix4x4 local = to_transform(it->position, it->rotation, it->scale);
                hierarchy.local.add(local);

                if (data->hierarchy_level == 0) {
                    last_root = (u32)hierarchy.local.length - 1;

                    hierarchy.results.add(local);
                    it->transform = local;
                }
                else if (data->hierarchy_level == 1) {
                    last_sub = (u32)hierarchy.local.length - 1;

                    Matrix4x4 result = multiply(hierarchy.results[last_root], local);
                    hierarchy.results.add(result);
                    it->transform = result;
                }
                else {
                    Matrix4x4 result = multiply(hierarchy.results[last_sub], local);
                    hierarchy.results.add(result);
                    it->transform = result;
                }
            }
            else {
                it->transform = to_transform(it->position, it->rotation, it->scale);
            }
        }}
    }
}

internal void render() {
    // Render all entities
    {
        Entity *it;
        bucket_array_foreach(game_state.entities.base_entities, it) {
            render_entity(*it, it->transform);
        }}
    }

    {
        glEnable(GL_DEPTH_TEST);
        glCullFace(GL_FRONT);

        glViewport(0, 0, SHADOW_SIZE, SHADOW_SIZE);
        glBindFramebuffer(GL_FRAMEBUFFER, game_state.light_data.frame_buffer);
        glClear(GL_DEPTH_BUFFER_BIT);

        set_shader(ShaderResource_Shadow);
        set_shader_matrix4x4("light", game_state.light_data.sun_mvp);

        flush_draw_calls_shadow();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(
            game_state.viewport.left,
            game_state.viewport.bottom,
            game_state.viewport.width,
            game_state.viewport.height
        );
        glCullFace(GL_BACK);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    {
        glEnable(GL_DEPTH_TEST);
        set_shader(ShaderResource_Default);
        set_shader_matrix4x4("view", game_state.camera.transform);
        set_shader_matrix4x4("projection", game_state.camera.perspective);
        set_shader_vec3("camera_position", game_state.camera.position);
        set_shader_sampler("shadow_texture", 1, game_state.light_data.shadow_texture);
        set_shader_matrix4x4("sun", game_state.light_data.sun_mvp);
        set_shader_vec3("sun_dir", game_state.light_data.sun_direction * -1.0f);

        flush_draw_calls();
    }

    {
        glDisable(GL_DEPTH_TEST);
        set_shader(ShaderResource_2D);
        set_shader_matrix4x4("projection", game_state.ortho_proj);

        set_shader_int("diffuse_alpha_mask", 0);
        flush_2d_shapes_draw_calls();

        set_shader_int("diffuse_alpha_mask", 1);
        flush_font_draw_calls();
    }
}
