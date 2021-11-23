#if !defined(MATH_H)
#define MATH_H

#define PI 3.14159f

#define TO_RADIANS(v) ((v) * PI / 180.0f)
#define TO_DEGREES(v) ((v) * 180.0f / PI)

inline f32 lerp(f32 a, f32 b, f32 t) {
    return (a + ((b - a) * t));
}

#endif
