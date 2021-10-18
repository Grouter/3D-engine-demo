#include "vector.h"

//
// Vector2
//

inline Vector2 make_vector2(f32 x, f32 y) {
    Vector2 result = { x, y };

    return result;
}

inline Vector2 make_vector2() {
    Vector2 result = { 0, 0 };

    return result;
}

//
// Vector3
//

const Vector3 V3_UP = Vector3 { 0.0f, 1.0f, 0.0f };

inline Vector3 make_vector3(f32 x, f32 y, f32 z) {
    Vector3 result = { x, y, z };

    return result;
}

inline Vector3 make_vector3() {
    Vector3 result = { 0, 0, 0 };

    return result;
}

inline void normalize(Vector3 &v) {
    float w = sqrt( v.x * v.x + v.y * v.y + v.z * v.z );

    if (w == 0)
        return;
        
    v.x /= w;
    v.y /= w;
    v.z /= w;
}

inline Vector3 normalized(Vector3 v) {

    normalize(v);

    return v;
}

inline Vector3 cross(Vector3 a, Vector3 b) {
    Vector3 cross;

    cross.x = (a.y * b.z) - (a.z * b.y);
    cross.y = (a.z * b.x) - (a.x * b.z);
    cross.z = (a.x * b.y) - (a.y * b.x);

    return cross;
}

inline Vector3 multiply(Vector3 v, f32 x) {
    return { v.x * x, v.y * x, v.z * x };
}

inline Vector3 multiply(Vector3 a, Vector3 b) {
    return { a.x * b.x, a.y * b.y, a.z * b.z };
}

inline Vector3 add(Vector3 a, Vector3 b) {
    return { a.x + b.x, a.y + b.y, a.z + b.z };
}