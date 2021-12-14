#if !defined(MATH_H)
#define MATH_H

constexpr f32 PI              = 3.14159265358979f;
constexpr f32 TWO_PI          = PI * 2.0f;
constexpr f32 HALF_PI         = PI * 0.5f;
constexpr f32 ONE_AND_HALF_PI = PI * 1.5f;

#define TO_RADIANS(v) ((v) * PI / 180.0f)
#define TO_DEGREES(v) ((v) * 180.0f / PI)

inline f32 lerp(f32 a, f32 b, f32 t) {
    return (a + ((b - a) * t));
}

inline f32 rand_f() {
    return (f32)rand() / (f32)RAND_MAX;
}

inline f32 rand_f_range(f32 start, f32 end) {
    f32 delta = end - start;

    assert(delta > 0);  // Start must be less than end

    return (rand_f() * delta) + start;
}

#endif
