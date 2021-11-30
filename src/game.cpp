internal void init_game() {
    game_state.camera = create_camera(VIRTUAL_WINDOW_W, VIRTUAL_WINDOW_H, 80.0f);
    game_state.camera.rotation.y = 90.0f;
    game_state.camera.position.z = 10.0f;

    game_state.ortho_proj = ortho(-1.0f, 1.0f, VIRTUAL_WINDOW_W_2D, VIRTUAL_WINDOW_H_2D);

    init_resources(game_state.resources);
    load_material_file();
    load_mesh_file();

    allocate_entity_storage(game_state.entities);

    load_world_file(game_state.entities);

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
            if (it->type == EntityType_Bird) {
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
        }}
    }
}

internal void render() {
    // Render all entities
    {
        Entity *it;

        bucket_array_foreach(game_state.entities.base_entities, it) {
            render_entity(*it);
        }}
    }

    glEnable(GL_DEPTH_TEST);
    flush_draw_calls();

    glDisable(GL_DEPTH_TEST);
    flush_2d_shapes_draw_calls();
    flush_font_draw_calls();
}
