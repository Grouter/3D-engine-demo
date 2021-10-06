inline void allocate_entity_storage(EntityStorage &storage) {
    allocate_bucket_array<Entity>(storage.base_entities, 10);
}

internal Entity* create_base_entity(EntityStorage &storage) {
    Entity e = {};

    BucketLocation location = storage.base_entities.add(e);

    return storage.base_entities.get(location);
}