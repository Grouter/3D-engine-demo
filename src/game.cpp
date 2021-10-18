internal void init() {
    game_state.camera = create_camera(VIRTUAL_WINDOW_W, VIRTUAL_WINDOW_H, 80.0f);
    game_state.camera.rotation.yaw = 90.0f;
    game_state.camera.position.z = 10.0f;

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
}

// @Temporary: testing
internal void rotate_entity(Entity &entity) {
    entity.rotation.y += 0.01f;
}

internal void tick() {
    camera_update(game_state.camera);

    game_state.entities.base_entities.for_each(rotate_entity);
}

internal void render() {
    render_entities();

    flush_draw_calls();
}
