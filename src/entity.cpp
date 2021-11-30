inline void allocate_entity_storage(EntityStorage &storage) {
    allocate_bucket_array<Entity>(storage.base_entities, 10);
    allocate_bucket_array<EntityData>(storage.entity_data, 10);
}

internal Entity* create_base_entity(EntityStorage &storage, EntityType type = EntityType_Basic) {
    Entity *entity;

    BucketLocation location = storage.base_entities.get_new(&entity);

    *entity = {};
    entity->type = type;

    return entity;
}

internal EntityData* create_entity_data(EntityStorage &storage, Entity &base) {
    EntityData *data;

    BucketLocation data_location = storage.entity_data.get_new(&data);
    *data = {};

    base.data = data_location;

    return data;
}

internal Entity* create_bird(EntityStorage &storage) {
    Entity *bird_entity = create_base_entity(storage, EntityType_Bird);

    create_entity_data(storage, *bird_entity);

    return bird_entity;
}