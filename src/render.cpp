// @Temporary: super very simple rendering
internal void render_entity(Entity &entity) {
    Matrix4x4 model = identity();
    translate(model, entity.position.x, entity.position.y, entity.position.z);
    rotate(model, entity.rotation.x, entity.rotation.y, entity.rotation.z);

    i32 model_handle = glGetUniformLocation(entity.program->handle, "model");
    glUniformMatrix4fv(model_handle, 1, GL_FALSE, model.raw);

    glBindVertexArray(entity.mesh->vao);
    glDrawElements(GL_TRIANGLES, (i32)entity.mesh->indicies.length, GL_UNSIGNED_INT, NULL);
}