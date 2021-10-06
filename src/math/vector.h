#if !defined(VECTOR_H)
#define VECTOR_H

#include "platform.h"

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
        f32 roll;
        f32 pitch;
        f32 yaw;
    };
    f32 data[3];
};

#endif
