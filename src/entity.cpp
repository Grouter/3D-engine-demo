inline void allocate_entity_storage(EntityStorage &storage) {
    allocate_bucket_array<Entity>(storage.base_entities, 10);
}

internal void render_entities() {
    set_shader(ShaderResource::Default);

    set_shader_view(game_state.camera.transform);
    set_shader_perspective(game_state.camera.perspective);

    game_state.entities.base_entities.for_each(render_entity);
}

internal Entity* create_base_entity(EntityStorage &storage) {
    Entity e = {};

    BucketLocation location = storage.base_entities.add(e);

    return storage.base_entities.get(location);
}