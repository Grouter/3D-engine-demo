internal void init() {
    game_state.camera = create_camera(VIRTUAL_WINDOW_W, VIRTUAL_WINDOW_H, 80.0f);
    game_state.camera.rotation.yaw = 90.0f;
    game_state.camera.position.z = 10.0f;

    game_state.resources.programs[0] = load_program("shaders/default.glsl");

    game_state.resources.meshes[0]   = load_model("monkey.obj");
    game_state.resources.meshes[1]   = load_model("cube.obj");

    game_state.resources.textures[0] = load_texture("1.jpg");
    game_state.resources.textures[1] = load_texture("2.png");

    // Send camera perspective to the shader uniform
    {
        glUseProgram(game_state.resources.programs[0].handle);
        i32 p = glGetUniformLocation(game_state.resources.programs[0].handle, "projection");
        glUniformMatrix4fv(p, 1, false, game_state.camera.perspective.raw);
        glUseProgram(0);
    }

    allocate_entity_storage(game_state.entities);

    {
        Entity *e = create_base_entity(game_state.entities);
        e->mesh = &game_state.resources.meshes[0];
        e->program = &game_state.resources.programs[0];
    }

    {
        Entity *e = create_base_entity(game_state.entities);
        e->mesh = &game_state.resources.meshes[1];
        e->program = &game_state.resources.programs[0];

        e->position.x = 10.0f;
    }
}

// @Temporary: testing
internal void rotate_entity(Entity &entity) {
    entity.rotation.y += 0.01f;
}

internal void tick() {
    camera_update(game_state.camera, input_state);

    game_state.entities.base_entities.for_each(rotate_entity);
}

internal void render() {
    glUseProgram(game_state.resources.programs[0].handle);

    {   // Update view uniform
        i32 view_handle = glGetUniformLocation(game_state.resources.programs[0].handle, "view");
        glUniformMatrix4fv(view_handle, 1, GL_FALSE, game_state.camera.transform.raw);
    }

    set_material_color(game_state.resources.programs[0], make_vector3(0.0f, 1.0f, 0.0f));

    glUniform1i(glGetUniformLocation(game_state.resources.programs[0].handle, "diffuse_texture"), 0);

    game_state.entities.base_entities.for_each(render_entity);
}
