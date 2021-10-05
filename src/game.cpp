internal void init() {
    game_state.camera = create_camera(game_state.window_width, game_state.window_height, 80.0);
    game_state.camera.position.z = -60.0f;

    game_state.resources.programs[0] = load_program("shaders/default.glsl");
    game_state.resources.meshes[0]   = create_cube(100.0f);

    // @Temporary
    glUseProgram(game_state.resources.programs[0].handle);
    i32 p = glGetUniformLocation(game_state.resources.programs[0].handle, "projection");
    glUniformMatrix4fv(p, 1, false, game_state.camera.perspective.raw);
}

internal void tick() {

}

internal void render() {
    glUseProgram(game_state.resources.programs[0].handle);

    i32 view_handle = glGetUniformLocation(game_state.resources.programs[0].handle, "view");
    Matrix4x4 view = identity();
    translate(view, game_state.camera.position.x, game_state.camera.position.y, game_state.camera.position.z);
    glUniformMatrix4fv(view_handle, 1, GL_FALSE, view.raw);

    glBindVertexArray(game_state.resources.meshes[0].vao);
    glDrawElements(GL_TRIANGLES, game_state.resources.meshes[0].index_count, GL_UNSIGNED_INT, 0);
}
