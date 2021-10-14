internal void set_material_color(Program &bound_program, Vector3 color) {
    i32 loc = glGetUniformLocation(bound_program.handle, "material_color");

    if (loc >= 0) {
        glUniform3f(loc, color.r, color.g, color.b);
    }
    else {
        printf("Shader set material_color loc error!\n");
    }
}

// @Temporary: super very simple rendering
internal void render_entity(Entity &entity) {
    Matrix4x4 model = identity();
    translate(model, entity.position.x, entity.position.y, entity.position.z);

    // @Todo: quaternions here!
    rotate(model, entity.rotation.x, entity.rotation.y, entity.rotation.z);

    i32 model_handle = glGetUniformLocation(entity.program->handle, "model");
    glUniformMatrix4fv(model_handle, 1, GL_FALSE, model.raw);

    Mesh *mesh = entity.mesh;

    glBindVertexArray(mesh->vao);

    for (u32 i = 0; i < mesh->sub_meshes.length; i++) {
        SubMeshInfo *info = &mesh->sub_meshes.data[i];

        // @Todo: switch materials and textures here...

        // Draw!
        u64 offset = info->first_index * info->index_count;
        glDrawElements(GL_TRIANGLES, (i32)entity.mesh->indicies.length, GL_UNSIGNED_INT, (const void *)offset);
    }

}