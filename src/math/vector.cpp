#include "vector.h"

inline Vector2 make_vector2(f32 x, f32 y) {
    Vector2 result = { x, y };

    return result;
}

inline Vector3 make_vector3(f32 x, f32 y, f32 z) {
    Vector3 result = { x, y, z };

    return result;
}
