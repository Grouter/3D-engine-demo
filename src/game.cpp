internal void init_game() {
    game_state.camera = create_camera(VIRTUAL_WINDOW_W, VIRTUAL_WINDOW_H, 80.0f);
    game_state.camera.rotation.y = 90.0f;
    game_state.camera.position.z = 10.0f;

    game_state.ortho_proj = ortho(-1.0f, 1.0f, VIRTUAL_WINDOW_W_2D, VIRTUAL_WINDOW_H_2D);

    init_resources(game_state.resources);
    load_material_file();
    load_mesh_file();

    allocate_entity_storage(game_state.entities);
    
    {
        Entity *e = create_base_entity(game_state.entities);
        e->mesh = get_mesh("island");
        e->program = &game_state.resources.programs[0];
        e->scale = make_vector3(1.5f);
        e->position.y -= 1.0f;
    }
    {
        Entity *e = create_base_entity(game_state.entities);
        e->mesh = get_mesh("houses_left");
        e->program = &game_state.resources.programs[0];

        e->position.x -= 10.0f;
    }
    {
        Entity *e = create_base_entity(game_state.entities);
        e->mesh = get_mesh("houses_right");
        e->program = &game_state.resources.programs[0];

        e->position.x += 10.0f;
    }
    {
        Entity *e = create_base_entity(game_state.entities);
        e->mesh = get_mesh("alchym");
        e->program = &game_state.resources.programs[0];
    }
    {
        Entity *e = create_base_entity(game_state.entities);
        e->mesh = get_mesh("ship");
        e->program = &game_state.resources.programs[0];

        e->position.z += 60.0f;
    }
    {
        Entity *e = create_base_entity(game_state.entities);
        e->mesh = get_mesh("docks");
        e->program = &game_state.resources.programs[0];

        e->position.z += 47.0f;
        e->position.y -= 6.0f;
    }
    {
        Entity *e = create_base_entity(game_state.entities);
        e->mesh = get_mesh("lanterns");
        e->program = &game_state.resources.programs[0];

        e->position.z -= 5.0f;
        e->position.y += .5f;
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
            if (it->type == EntityType_Bird) {
                BirdData *bird_data = &game_state.entities.entity_data[it->data].bird_data;

                bird_data->hover_animation += dt * BIRD_HOVER_SPEED;
                it->position.y = BIRD_HEIGHTS + (sinf(bird_data->hover_animation) * BIRD_HOVER_AMPL);
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
