#include "matrix.h"

inline Matrix4x4 identity() {
    Matrix4x4 result = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    return result;
}

inline Matrix4x4 scale(f32 x, f32 y, f32 z) {
    Matrix4x4 result = {
        x, 0, 0, 0,
        0, y, 0, 0,
        0, 0, z, 0,
        0, 0, 0, 1
    };

    return result;
}

inline Matrix4x4 translation(f32 x, f32 y, f32 z) {
    Matrix4x4 result = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        x, y, z, 1
    };

    return result;
}

inline Matrix4x4 rotation_x(f32 x) {
    f32 c_x = cosf(x);
    f32 s_x = sinf(x);

    Matrix4x4 result = {
        1,    0,   0, 0,
        0,  c_x, s_x, 0,
        0, -s_x, c_x, 0,
        0,    0,   0, 1
    };

    return result;
}

inline Matrix4x4 rotation_y(f32 y) {
    f32 c_y = cosf(y);
    f32 s_y = sinf(y);

    Matrix4x4 result = {
        c_y,  0, -s_y, 0,
          0,  1,    0, 0,
        s_y,  0,  c_y, 0,
          0,  0,    0, 1
    };

    return result;
}

inline Matrix4x4 rotation_z(f32 z) {
    f32 c_z = cosf(z);
    f32 s_z = sinf(z);

    Matrix4x4 result = {
        c_z, -s_z, 0, 0,
        s_z,  c_z, 0, 0,
          0,    0, 1, 0,
          0,    0, 0, 1
    };

    return result;
}

// Multiplies matricies (in column major order), that means it will swap them and will do "bXa"
internal Matrix4x4 multiply(const Matrix4x4 &a, const Matrix4x4 &b) {
    Matrix4x4 result = identity();

    for (int i = 0; i < 4; i++) {
        result.data[i][0] = a.data[0][0] * b.data[i][0] + a.data[1][0] * b.data[i][1] + a.data[2][0] * b.data[i][2] + a.data[3][0] * b.data[i][3];
        result.data[i][1] = a.data[0][1] * b.data[i][0] + a.data[1][1] * b.data[i][1] + a.data[2][1] * b.data[i][2] + a.data[3][1] * b.data[i][3];
        result.data[i][2] = a.data[0][2] * b.data[i][0] + a.data[1][2] * b.data[i][1] + a.data[2][2] * b.data[i][2] + a.data[3][2] * b.data[i][3];
        result.data[i][3] = a.data[0][3] * b.data[i][0] + a.data[1][3] * b.data[i][1] + a.data[2][3] * b.data[i][2] + a.data[3][3] * b.data[i][3];
    }

    return result;
}

internal void translate(Matrix4x4 &matrix, f32 x, f32 y, f32 z) {
    matrix.data[3][0] += x;
    matrix.data[3][1] += y;
    matrix.data[3][2] += z;
}

internal void rotate(Matrix4x4 &matrix, f32 x, f32 y, f32 z) {
    Matrix4x4 x_rotation = rotation_x(x);
    Matrix4x4 y_rotation = rotation_x(y);
    Matrix4x4 z_rotation = rotation_x(z);

    matrix = multiply(matrix, x_rotation);
    matrix = multiply(matrix, y_rotation);
    matrix = multiply(matrix, z_rotation);
}
