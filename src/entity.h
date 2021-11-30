#if !defined(ENTITY_H)
#define ENTITY_H

#include "platform.h"

const f32 BIRD_HEIGHTS = 0.0f;
const f32 BIRD_SPEED = 1.0f;
const f32 BIRD_HOVER_SPEED = 0.5f;
const f32 BIRD_HOVER_AMPL = 0.2f;

enum EntityType {
    EntityType_None,

    EntityType_Basic,
    EntityType_Bird,
};

struct Entity {
    EntityType type  = EntityType_Basic;

    BucketLocation data;

    Vector3 position = {};
    Vector3 rotation = {};   // Radians
    Vector3 scale    = {};

    Program *program = nullptr;
    Mesh *mesh       = nullptr;
};

struct BirdData {
    Vector2 direction;
    Vector2 velocity;
    Vector3 fly_target;
    f32 hover_animation;
};

union EntityData {
    BirdData bird_data;
};

struct EntityStorage {
    BucketArray<Entity> base_entities;
    BucketArray<EntityData> entity_data;
};

#endif
