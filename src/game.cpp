internal void init() {
    resources.programs[0] = load_program("shaders/default.glsl");
    resources.meshes[0] = create_rectangle();
}

internal void tick() {

}

internal void render() {
    glUseProgram(resources.programs[0].handle);

    glBindVertexArray(resources.meshes[0].vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
