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
        e->mesh = get_mesh("monkey");
        e->program = &game_state.resources.programs[0];
    }

    {
        Entity *e = create_base_entity(game_state.entities);
        e->mesh = get_mesh("cube");
        e->program = &game_state.resources.programs[0];

        e->position.x = 10.0f;
    }

    {
        Entity *e = create_base_entity(game_state.entities);
        e->mesh = get_mesh("cube");
        e->program = &game_state.resources.programs[0];

        e->position.x = -10.0f;
    }

    // Uncomment for animationzz
    // camera_start_animation(game_state.camera);
}

// @Temporary: testing
internal void rotate_entity(Entity &entity) {
    entity.rotation.y += 0.01f;
}

internal void tick(f32 dt) {
    camera_handle_input(game_state.camera);

    camera_animate(game_state.camera, game_state.resources.camera_animation, dt);
    camera_update(game_state.camera);

    game_state.entities.base_entities.for_each(rotate_entity);
}

internal void render() {
    render_entities();

    glEnable(GL_DEPTH_TEST);
    flush_draw_calls();

    glDisable(GL_DEPTH_TEST);
    flush_2d_shapes_draw_calls();
    flush_font_draw_calls();
}
