#if !defined(VECTOR_H)
#define VECTOR_H

union Vector2 {
    struct {
        f32 x;
        f32 y;
    };
    f32 data[2];
};

union Vector3 {
    struct {
        f32 x;
        f32 y;
        f32 z;
    };
    struct {
        f32 r;
        f32 g;
        f32 b;
    };
    struct {
        f32 pitch;
        f32 yaw;
        f32 roll;
    };
    f32 data[3];
};

union Vector4 {
    struct {
        f32 x;
        f32 y;
        f32 z;
        f32 w;
    };
    struct {
        f32 r;
        f32 g;
        f32 b;
        f32 a;
    };
    f32 data[4];
};

//
// :Vector2
//

const Vector2 V2_ONE = { 1.0f, 1.0f };

inline Vector2 make_vector2(f32 x, f32 y) {
    Vector2 result = { x, y };

    return result;
}

inline Vector2 operator* (Vector2 a, f32 b) {
    Vector2 result;

    result.x = a.x * b;
    result.y = a.y * b;

    return result;
}

inline Vector2 rand_unit_v2() {
    f32 a = rand_f_range(0.0f, TWO_PI);

    Vector2 result = make_vector2(cosf(a), sinf(a));

    return result;
}

//
// :Vector3
//

const Vector3 V3_ONE = { 1.0f, 1.0f, 1.0f };
const Vector3 V3_UP = { 0.0f, 1.0f, 0.0f };

inline Vector3 make_vector3(f32 x, f32 y, f32 z) {
    Vector3 result = { x, y, z };

    return result;
}

inline Vector3 make_vector3(f32 v) {
    Vector3 result = { v, v, v };

    return result;
}

inline f32 length(Vector3 v) {
    f32 result = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);

    return result;
}

inline void normalize(Vector3 &v) {
    float l = length(v);

    if (l == 0)
        return;

    v.x /= l;
    v.y /= l;
    v.z /= l;
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

inline Vector3 operator* (Vector3 a, f32 b) {
    Vector3 result;

    result.x = a.x * b;
    result.y = a.y * b;
    result.z = a.z * b;

    return result;
}

inline Vector3 operator*= (Vector3 &a, f32 b) {
    a.x *= b;
    a.y *= b;
    a.z *= b;

    return a;
}

inline Vector3 operator/= (Vector3 &a, f32 b) {
    a.x /= b;
    a.y /= b;
    a.z /= b;

    return a;
}

inline Vector3 operator+ (Vector3 a, Vector3 b) {
    Vector3 result;

    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;

    return result;
}

inline Vector3 operator+ (Vector3 a, f32 b) {
    Vector3 result;

    result.x = a.x + b;
    result.y = a.y + b;
    result.z = a.z + b;

    return result;
}

inline Vector3 operator+= (Vector3 &a, Vector3 b) {
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;

    return a;
}

inline Vector3 operator-= (Vector3 &a, Vector3 b) {
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;

    return a;
}

inline Vector3 operator- (Vector3 a, Vector3 b) {
    Vector3 result;

    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;

    return result;
}

inline Vector3 lerp(Vector3 a, Vector3 b, f32 t) {
    Vector3 result;

    result.x = lerp(a.x, b.x, t);
    result.y = lerp(a.y, b.y, t);
    result.z = lerp(a.z, b.z, t);

    return result;
}

inline void limit(Vector3 &v, f32 max_length) {
    f32 l = length(v);

    if (l <= max_length) return;

    normalize(v);
    v *= max_length;
}

inline f32 distance(Vector3 a, Vector3 b) {
    Vector3 line = b - a;

    f32 result = length(line);

    return result;
}

inline void dump_vector(Vector3 &v) {
    log_print("Vector3: %f %f %f\n", v.x, v.y, v.z);
}

//
// :Vector4
//

const Vector4 V4_ONE = { 1.0f, 1.0f, 1.0f, 1.0f };

inline Vector4 make_vector4(f32 x, f32 y, f32 z, f32 w) {
    Vector4 result = { x, y, z, w };

    return result;
}

inline void dump_vector(Vector4 &v) {
    log_print("Vector4: %f %f %f %f\n", v.x, v.y, v.z, v.w);
}

#endif
