#if !defined(MATRIX_H)
#define MATRIX_H

#include "platform.h"

union Matrix4x4 {
    f32 table[4][4];
    f32 raw[16];
};

#endif
