internal void init() {
    game_state.camera = create_camera(game_state.window_width, game_state.window_height, 80.0);
    game_state.camera.position.z = -20.0f;

    game_state.resources.programs[0] = load_program("shaders/default.glsl");
    game_state.resources.meshes[0]   = load_model("test.obj");

    allocate_entity_storage(game_state.entities);

    {
        Entity *e = create_base_entity(game_state.entities);
        e->mesh = &game_state.resources.meshes[0];
        e->program = &game_state.resources.programs[0];
    }

    {
        Entity *e = create_base_entity(game_state.entities);
        e->mesh = &game_state.resources.meshes[0];
        e->program = &game_state.resources.programs[0];

        e->position.x = 50.0f;
        e->position.z = -50.0f;
    }

    // @Temporary
    glUseProgram(game_state.resources.programs[0].handle);
    i32 p = glGetUniformLocation(game_state.resources.programs[0].handle, "projection");
    glUniformMatrix4fv(p, 1, false, game_state.camera.perspective.raw);
}

// @Temporary: testing
internal void rotate_entity(Entity &entity) {
    entity.rotation.y += 0.01f;
}

internal void tick() {
    game_state.entities.base_entities.for_each(rotate_entity);
}

internal void render() {
    glUseProgram(game_state.resources.programs[0].handle);

    {   // Update view uniform
        Matrix4x4 view = identity();
        translate(view, game_state.camera.position.x, game_state.camera.position.y, game_state.camera.position.z);

        i32 view_handle = glGetUniformLocation(game_state.resources.programs[0].handle, "view");
        glUniformMatrix4fv(view_handle, 1, GL_FALSE, view.raw);
    }

    game_state.entities.base_entities.for_each(render_entity);
}
