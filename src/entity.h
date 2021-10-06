#if !defined(ENTITY_H)
#define ENTITY_H

#include "platform.h"

struct Entity {
    Vector3 position;
    Vector3 rotation;   // Radians
    Vector3 scale;

    Program *program;
    Mesh *mesh;
};

struct EntityStorage {
    BucketArray<Entity> base_entities;
};

#endif
