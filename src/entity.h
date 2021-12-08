#if !defined(ENTITY_H)
#define ENTITY_H

#include "platform.h"

const u32 ROCKS          = 50;
const u32 SUB_ROCKS      = 5;
const u32 SUB_SUB_ROCKS  = 10;

constexpr u32 ROCK_COUNT = ROCKS * SUB_ROCKS * SUB_SUB_ROCKS;

const f32 BIRD_HEIGHTS = 50.0f;
const f32 BIRD_SPEED = 8.0f;
const f32 BIRD_HOVER_SPEED = 1.5f;
const f32 BIRD_HOVER_AMPL = 0.2f;
const f32 BIRD_LIFESPAN = 240.0f;
const f32 BIRD_SPAWN_TIME = 5.0f;

enum EntityType {
    EntityType_NONE,

    EntityType_BASIC,
    EntityType_BIRD,
    EntityType_FLYING_ROCK,
    EntityType_SHIP,
};

union EntityFlags {
    u32 raw = 0;

    struct {
        u8 destroy;
    };
};

struct Entity {
    EntityFlags flags;
    EntityType type  = EntityType_BASIC;

    BucketLocation data;

    Vector3 position = {};
    Vector3 rotation = {};   // Radians
    Vector3 scale    = V3_ONE;
    Matrix4x4 transform;

    Program *program = nullptr;
    Mesh *mesh       = nullptr;
};

struct ShipData {
    f32 original_y_position = FLT_MAX;
};

struct BirdData {
    Vector2 direction;
    f32 life;
};

struct FlyingRockData {
    f32 rotation_direction = 1.0f;
    i32 hierarchy_level = 0;
};

union EntityData {
    FlyingRockData flying_rock_data;
    BirdData bird_data;
    ShipData ship_data;
};

struct FlyingRockTransformHierarchy {
    Array<Matrix4x4> local;
    Array<i32> lookups;
    Array<Matrix4x4> results;
};

struct EntityStorage {
    BucketArray<Entity> base_entities;
    BucketArray<EntityData> entity_data;

    FlyingRockTransformHierarchy flying_rock_transforms;
};

internal Entity* create_entity_from_type(EntityStorage &storage, EntityType type);
internal void remove_flagged_entities(EntityStorage &storage);

#endif
