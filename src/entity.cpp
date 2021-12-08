inline void allocate_entity_storage(EntityStorage &storage) {
    allocate_bucket_array<Entity>(storage.base_entities, 10);
    allocate_bucket_array<EntityData>(storage.entity_data, 10);

    allocate_array(storage.flying_rock_transforms.local, ROCK_COUNT);
    allocate_array(storage.flying_rock_transforms.lookups, ROCK_COUNT);
    allocate_array(storage.flying_rock_transforms.results, ROCK_COUNT);
}

internal void remove_flagged_entities(EntityStorage &storage) {
    Entity *e;
    BucketLocation location;

    bucket_array_foreach_w_loc(storage.base_entities, e, location) {
        if (!e->flags.destroy) continue;

        switch (e->type) {
            case EntityType_BIRD :
            case EntityType_SHIP :
            case EntityType_FLYING_ROCK : {
                storage.entity_data.remove(e->data);
            } break;
        }

        storage.base_entities.remove(location);
    }}
}

internal Entity* create_base_entity(EntityStorage &storage, EntityType type = EntityType_BASIC) {
    Entity *entity;

    BucketLocation location = storage.base_entities.get_new(&entity);

    *entity = {};
    entity->program = &game_state.resources.programs[ShaderResource_Default];
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
    Entity *bird_entity = create_base_entity(storage, EntityType_BIRD);

    bird_entity->mesh = get_mesh("bird");

    create_entity_data(storage, *bird_entity);

    return bird_entity;
}

internal Entity* create_flying_rock(EntityStorage &storage, i32 level = 0) {
    Entity *root = create_base_entity(storage, EntityType_FLYING_ROCK);

    // Pls don't... Don't look here!
    {
        u32 r = rand() % 7;
        switch (r) {
            case 0 : root->mesh = get_mesh("rock1"); break;
            case 1 : root->mesh = get_mesh("rock2"); break;
            case 2 : root->mesh = get_mesh("rock3"); break;
            case 3 : root->mesh = get_mesh("rock4"); break;
            case 4 : root->mesh = get_mesh("rock5"); break;
            case 5 : root->mesh = get_mesh("rock6"); break;
            case 6 : root->mesh = get_mesh("rock7"); break;
        }
    }

    root->program = &game_state.resources.programs[0];

    FlyingRockData *data = &create_entity_data(storage, *root)->flying_rock_data;

    {
        u32 r = rand() % 2;
        if (r == 0) data->rotation_direction = -1.0f;
        else data->rotation_direction = 1.0f;
    }

    data->hierarchy_level = level;

    return root;
}

// Returns the root
internal Entity* create_rock_formation(EntityStorage &storage) {
    Entity *root = create_flying_rock(storage);

    u32 sub_count = (rand() % SUB_ROCKS) + 1;

    // Spawn sub rocks
    for (u32 i = 0; i < sub_count; i++) {
        Entity *sub = create_flying_rock(storage, 1);

        Vector2 sub_offset = rand_unit_v2();
        sub->position.x = sub_offset.x * 3.0f;
        sub->position.z = sub_offset.y * 3.0f;
        sub->position.y = rand_f_range(-3.0f, 3.0f);

        sub->scale = make_vector3(0.5f);

        // Spawn sub sub rocks
        u32 sub_sub_count = (rand() % SUB_SUB_ROCKS) + 1;

        for (u32 j = 0; j < sub_sub_count; j++) {
            Entity *sub_sub = create_flying_rock(storage, 2);

            Vector2 sub_sub_offset = rand_unit_v2();
            sub_sub->position.x = sub_sub_offset.x * 2.0f;
            sub_sub->position.z = sub_sub_offset.y * 2.0f;
            sub_sub->position.y = rand_f_range(-3.0f, 3.0f);

            sub_sub->scale = make_vector3(0.15f);
        }
    }

    return root;
}

internal Entity* create_ship(EntityStorage &storage) {
    Entity *root = create_base_entity(storage, EntityType_SHIP);

    ShipData *data = &create_entity_data(storage, *root)->ship_data;

    *data = {};

    return root;
}

internal Entity* create_tree(EntityStorage &storage) {
    Entity *tree = create_base_entity(storage, EntityType_TREE);

    tree->program = &game_state.resources.programs[ShaderResource_Trees];

    return tree;
}

internal Entity* create_entity_from_type(EntityStorage &storage, EntityType type) {
    Entity *result;

    switch (type) {
        case EntityType_NONE : {
            log_print("Cannot instantiate NONE entity!\n");
            return nullptr;
        } break;

        case EntityType_BASIC : {
            result = create_base_entity(storage);
        } break;

        case EntityType_BIRD : {
            result = create_bird(storage);
        } break;

        case EntityType_SHIP : {
            result = create_ship(storage);
        } break;

        case EntityType_TREE : {
            result = create_tree(storage);
        } break;

        default : {
            log_print("Unknown entity in create_entity_from_type\n");
            return nullptr;
        } break;
    }

    return result;
}