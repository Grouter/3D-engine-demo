internal void init() {
    game_state.camera = create_camera(game_state.window_width, game_state.window_height, 80.0);
    game_state.camera.position.z = -60.0f;

    game_state.resources.programs[0] = load_program("shaders/default.glsl");
    game_state.resources.meshes[0]   = create_cube(50.0f);

    allocate_bucket_array<Entity>(game_state.entities, 10); // @Temporary count
    game_state.entities.add(Entity {});

    // @Temporary
    glUseProgram(game_state.resources.programs[0].handle);
    i32 p = glGetUniformLocation(game_state.resources.programs[0].handle, "projection");
    glUniformMatrix4fv(p, 1, false, game_state.camera.perspective.raw);
}

internal void tick() {
    game_state.entities.get(BucketLocation { 0, 0 })->rotation.y += 0.01f;
}

internal void render() {
    glUseProgram(game_state.resources.programs[0].handle);

    {   // Update view uniform
        Matrix4x4 view = identity();
        translate(view, game_state.camera.position.x, game_state.camera.position.y, game_state.camera.position.z);

        i32 view_handle = glGetUniformLocation(game_state.resources.programs[0].handle, "view");
        glUniformMatrix4fv(view_handle, 1, GL_FALSE, view.raw);
    }

    {   // Render the test model
        Entity *e = game_state.entities.get(BucketLocation { 0, 0 });

        Matrix4x4 model = identity();
        translate(model, e->position.x, e->position.y, e->position.z);
        rotate(model, e->rotation.x, e->rotation.y, e->rotation.z);

        i32 model_handle = glGetUniformLocation(game_state.resources.programs[0].handle, "model");
        glUniformMatrix4fv(model_handle, 1, GL_FALSE, model.raw);

        glBindVertexArray(game_state.resources.meshes[0].vao);
        glDrawElements(GL_TRIANGLES, game_state.resources.meshes[0].index_count, GL_UNSIGNED_INT, NULL);
    }
}
