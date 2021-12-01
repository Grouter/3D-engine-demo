internal void init_game() {
    srand(time(0));

    game_state.time_elapsed = 0.0f;

    game_state.camera = create_camera(VIRTUAL_WINDOW_W, VIRTUAL_WINDOW_H, 80.0f);
    game_state.camera.rotation.y = 90.0f;
    game_state.camera.position.z = 10.0f;

    game_state.ortho_proj = ortho(-1.0f, 1.0f, VIRTUAL_WINDOW_W_2D, VIRTUAL_WINDOW_H_2D);

    init_resources(game_state.resources);
    load_material_file();
    load_mesh_file();

    allocate_entity_storage(game_state.entities);

    load_world_file(game_state.entities);

    {
        f32 a_part = TWO_PI / ROCKS;
        f32 current_a = 0.0f;

        for (i32 i = 0; i < ROCKS; i++) {
            Entity *root = create_rock_formation(game_state.entities);

            f32 radius = rand_f_range(100, 200);

            root->position.x = sinf(current_a) * radius;
            root->position.y = rand_f_range(-50, 150);
            root->position.z = cosf(current_a) * radius;
            root->scale = make_vector3(5.0f);

            current_a += a_part;
        }
    }

    // Uncomment for animationzz
    // camera_start_animation(game_state.camera);
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
}

internal void render() {
    // Calculate rocks hierarchical transforms
    {
        FlyingRockTransformHierarchy &hierarchy = game_state.entities.flying_rock_transforms;

        hierarchy.local.clear();
        hierarchy.lookups.clear();
        hierarchy.results.clear();

        u32 last_root = 0;
        u32 last_sub = 0;

        Entity *it;
        bucket_array_foreach(game_state.entities.base_entities, it) {
            if (it->type != EntityType_FLYING_ROCK) continue;

            FlyingRockData *data = &game_state.entities.entity_data[it->data].flying_rock_data;

            Matrix4x4 local = from(it->position, it->rotation, it->scale);
            hierarchy.local.add(local);

            if (data->hierarchy_level == 0) {
                last_root = (u32)hierarchy.local.length - 1;

                hierarchy.results.add(local);
            }
            else if (data->hierarchy_level == 1) {
                last_sub = (u32)hierarchy.local.length - 1;

                Matrix4x4 result = multiply(hierarchy.results[last_root], local);
                hierarchy.results.add(result);
            }
            else {
                Matrix4x4 result = multiply(hierarchy.results[last_sub], local);
                hierarchy.results.add(result);
            }
        }}
    }


    // Render all entities
    {
        Entity *it;
        u32 rock_index = 0;

        bucket_array_foreach(game_state.entities.base_entities, it) {
            if (it->type == EntityType_FLYING_ROCK) {
                render_entity(*it, game_state.entities.flying_rock_transforms.results[rock_index]);
                rock_index += 1;
            }
            else {
                Matrix4x4 transform = from(it->position, it->rotation, it->scale);
                render_entity(*it, transform);
            }
        }}
    }

    glEnable(GL_DEPTH_TEST);
    flush_draw_calls();

    glDisable(GL_DEPTH_TEST);
    flush_2d_shapes_draw_calls();
    flush_font_draw_calls();
}
