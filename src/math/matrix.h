#if !defined(MATRIX_H)
#define MATRIX_H

union Matrix4x4 {
    f32 table[4][4];
    f32 raw[16];
};

inline Matrix4x4 identity() {
    Matrix4x4 result = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    return result;
}

inline Matrix4x4 scaling(f32 x, f32 y, f32 z) {
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
    f32 c_x = cos(x);
    f32 s_x = sin(x);

    Matrix4x4 result = {
        1,    0,   0, 0,
        0,  c_x, s_x, 0,
        0, -s_x, c_x, 0,
        0,    0,   0, 1
    };

    return result;
}

inline Matrix4x4 rotation_y(f32 y) {
    f32 c_y = cos(y);
    f32 s_y = sin(y);

    Matrix4x4 result = {
        c_y,  0, -s_y, 0,
          0,  1,    0, 0,
        s_y,  0,  c_y, 0,
          0,  0,    0, 1
    };

    return result;
}

inline Matrix4x4 rotation_z(f32 z) {
    f32 c_z = cos(z);
    f32 s_z = sin(z);

    Matrix4x4 result = {
        c_z, -s_z, 0, 0,
        s_z,  c_z, 0, 0,
          0,    0, 1, 0,
          0,    0, 0, 1
    };

    return result;
}

inline Matrix4x4 perspective(f32 fov, f32 clip_near, f32 clip_far, f32 display_w, f32 display_h) {
    f32 aspect = display_w / display_h;
    f32 fov_radians = TO_RADIANS(fov);
    f32 cotan_half_fov = 1.0f / tan(fov_radians / 2.0f);

    Matrix4x4 perspective = {};

    perspective.table[0][0] = cotan_half_fov / aspect;
    perspective.table[0][1] = 0.0f;
    perspective.table[0][2] = 0.0f;
    perspective.table[0][3] = 0.0f;

    perspective.table[1][0] = 0.0f;
    perspective.table[1][1] = cotan_half_fov;
    perspective.table[1][2] = 0.0f;
    perspective.table[1][3] = 0.0f;

    perspective.table[2][0] = 0.0f;
    perspective.table[2][1] = 0.0f;
    perspective.table[2][2] = (clip_far + clip_near) / (clip_near - clip_far);
    perspective.table[2][3] = -1.0f;

    perspective.table[3][0] =  0.0f;
    perspective.table[3][1] =  0.0f;
    perspective.table[3][2] = (2.0f * clip_far * clip_near) / (clip_near - clip_far);
    perspective.table[3][3] =  0.0f;

    return perspective;
}

inline Matrix4x4 ortho(f32 clip_near, f32 clip_far, f32 display_w, f32 display_h) {
    f32 left   = 0.0f;
    f32 right  = display_w;
    f32 top    = display_h;
    f32 bottom = 0.0f;

    Matrix4x4 orto = {};

    orto.table[0][0] = 2.0f / (right - left);
    orto.table[0][1] = 0.0f;
    orto.table[0][2] = 0.0f;
    orto.table[0][3] = 0.0f;

    orto.table[1][0] = 0.0f;
    orto.table[1][1] = 2.0f / (top - bottom);
    orto.table[1][2] = 0.0f;
    orto.table[1][3] = 0.0f;

    orto.table[2][0] = 0.0f;
    orto.table[2][1] = 0.0f;
    orto.table[2][2] = 2.0f / (clip_far - clip_near);
    orto.table[2][3] = 0.0f;

    orto.table[3][0] = -(right + left) / (right - left);
    orto.table[3][1] = -(top + bottom) / (top - bottom);
    orto.table[3][2] = -(clip_far + clip_near) / (clip_far - clip_near);
    orto.table[3][3] = 1.0f;

    return orto;
}

inline Matrix4x4 ortho(f32 clip_near, f32 clip_far, f32 left, f32 right, f32 top, f32 bottom) {
    Matrix4x4 orto = {};

    orto.table[0][0] = 2.0f / (right - left);
    orto.table[0][1] = 0.0f;
    orto.table[0][2] = 0.0f;
    orto.table[0][3] = 0.0f;

    orto.table[1][0] = 0.0f;
    orto.table[1][1] = 2.0f / (top - bottom);
    orto.table[1][2] = 0.0f;
    orto.table[1][3] = 0.0f;

    orto.table[2][0] = 0.0f;
    orto.table[2][1] = 0.0f;
    orto.table[2][2] = 2.0f / (clip_far - clip_near);
    orto.table[2][3] = 0.0f;

    orto.table[3][0] = -(right + left) / (right - left);
    orto.table[3][1] = -(top + bottom) / (top - bottom);
    orto.table[3][2] = -(clip_far + clip_near) / (clip_far - clip_near);
    orto.table[3][3] = 1.0f;

    return orto;
}

// Multiplies matricies (in column major order), that means it will swap them and will do "bXa"
internal Matrix4x4 multiply(const Matrix4x4 &a, const Matrix4x4 &b) {
    Matrix4x4 result = identity();

    for (int i = 0; i < 4; i++) {
        result.table[i][0] = a.table[0][0] * b.table[i][0] + a.table[1][0] * b.table[i][1] + a.table[2][0] * b.table[i][2] + a.table[3][0] * b.table[i][3];
        result.table[i][1] = a.table[0][1] * b.table[i][0] + a.table[1][1] * b.table[i][1] + a.table[2][1] * b.table[i][2] + a.table[3][1] * b.table[i][3];
        result.table[i][2] = a.table[0][2] * b.table[i][0] + a.table[1][2] * b.table[i][1] + a.table[2][2] * b.table[i][2] + a.table[3][2] * b.table[i][3];
        result.table[i][3] = a.table[0][3] * b.table[i][0] + a.table[1][3] * b.table[i][1] + a.table[2][3] * b.table[i][2] + a.table[3][3] * b.table[i][3];
    }

    return result;
}

internal Vector3 multiply(const Matrix4x4 &a, Vector3 b) {
    Vector3 result;

    result.x = a.table[0][0] * b.x + a.table[1][0] * b.y + a.table[2][0] * b.z + a.table[3][0];
    result.y = a.table[0][1] * b.x + a.table[1][1] * b.y + a.table[2][1] * b.z + a.table[3][1];
    result.z = a.table[0][2] * b.x + a.table[1][2] * b.y + a.table[2][2] * b.z + a.table[3][2];

    return result;
}

inline void translate(Matrix4x4 &matrix, f32 x, f32 y, f32 z) {
    matrix.table[3][0] += x;
    matrix.table[3][1] += y;
    matrix.table[3][2] += z;
}

internal void rotate(Matrix4x4 &matrix, f32 x, f32 y, f32 z) {
    Matrix4x4 x_rotation = rotation_x(x);
    Matrix4x4 y_rotation = rotation_y(y);
    Matrix4x4 z_rotation = rotation_z(z);

    matrix = multiply(matrix, x_rotation);
    matrix = multiply(matrix, y_rotation);
    matrix = multiply(matrix, z_rotation);
}

inline void scale(Matrix4x4 &matrix, f32 x, f32 y, f32 z) {
    Matrix4x4 scale_m = scaling(x, y, z);

    matrix = multiply(matrix, scale_m);
}

inline Matrix4x4 transposed(Matrix4x4 &matrix) {
    return {
        matrix.table[0][0], matrix.table[1][0], matrix.table[2][0], matrix.table[3][0],
        matrix.table[0][1], matrix.table[1][1], matrix.table[2][1], matrix.table[3][1],
        matrix.table[0][2], matrix.table[1][2], matrix.table[2][2], matrix.table[3][2],
        matrix.table[0][3], matrix.table[1][3], matrix.table[2][3], matrix.table[3][3]
    };
}

inline Matrix4x4 to_transform(Vector3 position, Vector3 rotation, Vector3 size) {
    Matrix4x4 result = identity();

    scale(result, size.x, size.y, size.z);
    rotate(result, rotation.x, rotation.y, rotation.z);
    translate(result, position.x, position.y, position.z);

    return result;
}

// From MESA GLU
internal bool invert(const Matrix4x4 &m, Matrix4x4 &output) {
    output.raw[0] = m.raw[5]  * m.raw[10] * m.raw[15] -
                    m.raw[5]  * m.raw[11] * m.raw[14] -
                    m.raw[9]  * m.raw[6]  * m.raw[15] +
                    m.raw[9]  * m.raw[7]  * m.raw[14] +
                    m.raw[13] * m.raw[6]  * m.raw[11] -
                    m.raw[13] * m.raw[7]  * m.raw[10];

    output.raw[4] = -m.raw[4]  * m.raw[10] * m.raw[15] +
                    m.raw[4]  * m.raw[11] * m.raw[14] +
                    m.raw[8]  * m.raw[6]  * m.raw[15] -
                    m.raw[8]  * m.raw[7]  * m.raw[14] -
                    m.raw[12] * m.raw[6]  * m.raw[11] +
                    m.raw[12] * m.raw[7]  * m.raw[10];

    output.raw[8] = m.raw[4]  * m.raw[9] * m.raw[15] -
                    m.raw[4]  * m.raw[11] * m.raw[13] -
                    m.raw[8]  * m.raw[5] * m.raw[15] +
                    m.raw[8]  * m.raw[7] * m.raw[13] +
                    m.raw[12] * m.raw[5] * m.raw[11] -
                    m.raw[12] * m.raw[7] * m.raw[9];

    output.raw[12] = -m.raw[4]  * m.raw[9] * m.raw[14] +
                    m.raw[4]  * m.raw[10] * m.raw[13] +
                    m.raw[8]  * m.raw[5] * m.raw[14] -
                    m.raw[8]  * m.raw[6] * m.raw[13] -
                    m.raw[12] * m.raw[5] * m.raw[10] +
                    m.raw[12] * m.raw[6] * m.raw[9];

    output.raw[1] = -m.raw[1]  * m.raw[10] * m.raw[15] +
                    m.raw[1]  * m.raw[11] * m.raw[14] +
                    m.raw[9]  * m.raw[2] * m.raw[15] -
                    m.raw[9]  * m.raw[3] * m.raw[14] -
                    m.raw[13] * m.raw[2] * m.raw[11] +
                    m.raw[13] * m.raw[3] * m.raw[10];

    output.raw[5] = m.raw[0]  * m.raw[10] * m.raw[15] -
                    m.raw[0]  * m.raw[11] * m.raw[14] -
                    m.raw[8]  * m.raw[2] * m.raw[15] +
                    m.raw[8]  * m.raw[3] * m.raw[14] +
                    m.raw[12] * m.raw[2] * m.raw[11] -
                    m.raw[12] * m.raw[3] * m.raw[10];

    output.raw[9] = -m.raw[0]  * m.raw[9] * m.raw[15] +
                    m.raw[0]  * m.raw[11] * m.raw[13] +
                    m.raw[8]  * m.raw[1] * m.raw[15] -
                    m.raw[8]  * m.raw[3] * m.raw[13] -
                    m.raw[12] * m.raw[1] * m.raw[11] +
                    m.raw[12] * m.raw[3] * m.raw[9];

    output.raw[13] = m.raw[0]  * m.raw[9] * m.raw[14] -
                    m.raw[0]  * m.raw[10] * m.raw[13] -
                    m.raw[8]  * m.raw[1] * m.raw[14] +
                    m.raw[8]  * m.raw[2] * m.raw[13] +
                    m.raw[12] * m.raw[1] * m.raw[10] -
                    m.raw[12] * m.raw[2] * m.raw[9];

    output.raw[2] = m.raw[1]  * m.raw[6] * m.raw[15] -
                    m.raw[1]  * m.raw[7] * m.raw[14] -
                    m.raw[5]  * m.raw[2] * m.raw[15] +
                    m.raw[5]  * m.raw[3] * m.raw[14] +
                    m.raw[13] * m.raw[2] * m.raw[7] -
                    m.raw[13] * m.raw[3] * m.raw[6];

    output.raw[6] = -m.raw[0]  * m.raw[6] * m.raw[15] +
                    m.raw[0]  * m.raw[7] * m.raw[14] +
                    m.raw[4]  * m.raw[2] * m.raw[15] -
                    m.raw[4]  * m.raw[3] * m.raw[14] -
                    m.raw[12] * m.raw[2] * m.raw[7] +
                    m.raw[12] * m.raw[3] * m.raw[6];

    output.raw[10] = m.raw[0]  * m.raw[5] * m.raw[15] -
                    m.raw[0]  * m.raw[7] * m.raw[13] -
                    m.raw[4]  * m.raw[1] * m.raw[15] +
                    m.raw[4]  * m.raw[3] * m.raw[13] +
                    m.raw[12] * m.raw[1] * m.raw[7] -
                    m.raw[12] * m.raw[3] * m.raw[5];

    output.raw[14] = -m.raw[0]  * m.raw[5] * m.raw[14] +
                    m.raw[0]  * m.raw[6] * m.raw[13] +
                    m.raw[4]  * m.raw[1] * m.raw[14] -
                    m.raw[4]  * m.raw[2] * m.raw[13] -
                    m.raw[12] * m.raw[1] * m.raw[6] +
                    m.raw[12] * m.raw[2] * m.raw[5];

    output.raw[3] = -m.raw[1] * m.raw[6] * m.raw[11] +
                    m.raw[1] * m.raw[7] * m.raw[10] +
                    m.raw[5] * m.raw[2] * m.raw[11] -
                    m.raw[5] * m.raw[3] * m.raw[10] -
                    m.raw[9] * m.raw[2] * m.raw[7] +
                    m.raw[9] * m.raw[3] * m.raw[6];

    output.raw[7] = m.raw[0] * m.raw[6] * m.raw[11] -
                    m.raw[0] * m.raw[7] * m.raw[10] -
                    m.raw[4] * m.raw[2] * m.raw[11] +
                    m.raw[4] * m.raw[3] * m.raw[10] +
                    m.raw[8] * m.raw[2] * m.raw[7] -
                    m.raw[8] * m.raw[3] * m.raw[6];

    output.raw[11] = -m.raw[0] * m.raw[5] * m.raw[11] +
                    m.raw[0] * m.raw[7] * m.raw[9] +
                    m.raw[4] * m.raw[1] * m.raw[11] -
                    m.raw[4] * m.raw[3] * m.raw[9] -
                    m.raw[8] * m.raw[1] * m.raw[7] +
                    m.raw[8] * m.raw[3] * m.raw[5];

    output.raw[15] = m.raw[0] * m.raw[5] * m.raw[10] -
                    m.raw[0] * m.raw[6] * m.raw[9] -
                    m.raw[4] * m.raw[1] * m.raw[10] +
                    m.raw[4] * m.raw[2] * m.raw[9] +
                    m.raw[8] * m.raw[1] * m.raw[6] -
                    m.raw[8] * m.raw[2] * m.raw[5];

    f32 det = m.raw[0] * output.raw[0] + m.raw[1] * output.raw[4] + m.raw[2] * output.raw[8] + m.raw[3] * output.raw[12];

    if (det == 0.0f) return false;

    det = 1.0f / det;

    for (i32 i = 0; i < 16; i++) output.raw[i] *= det;

    return true;
}

inline Matrix4x4 look_at(Vector3 position, Vector3 target) {
    Vector3 direction = target - position;

    Vector3 forward = normalized(direction);
    Vector3 side    = normalized(cross(V3_UP, forward));
    Vector3 up      = cross(forward, side);

    Vector3 result_position = make_vector3(
        -position.x * side.x    - position.y * side.y    - position.z * side.z,
        -position.x * up.x      - position.y * up.y      - position.z * up.z,
        -position.x * forward.x - position.y * forward.y - position.z * forward.z
    );

    return {
        side.x, up.x, forward.x, 0.0f,
        side.y, up.y, forward.y, 0.0f,
        side.z, up.z, forward.z, 0.0f,
        result_position.x, result_position.y, result_position.z, 1.0f,
    };
}

inline Vector3 get_forward_vector(Matrix4x4 &m) {
    return { m.raw[2], m.raw[6], m.raw[10] };
}

inline Vector3 get_up_vector(Matrix4x4 &m) {
    return { m.raw[1], m.raw[5], m.raw[9] };
}

inline Vector3 get_side_vector(Matrix4x4 &m) {
    return { m.raw[0], m.raw[4], m.raw[8] };
}

#endif
