#if !defined(QUATERNION_H)
#define QUATERNION_H

//
// Quaternion are always in radians and all further conversions are in radians too!
//

union Quaternion {
    f32 data[4];
    struct {
        f32 x;
        f32 y;
        f32 z;
        f32 w;
    };
};

inline Quaternion from_axis_angle(Vector3 axis, f32 rotation_radians) {
    f32 half_rotation = rotation_radians / 2.0f;
    f32 sin_half_rotation = sin(half_rotation);

    Quaternion result = {
        axis.x * sin_half_rotation,
        axis.y * sin_half_rotation,
        axis.z * sin_half_rotation,
        cos(half_rotation)
    };

    return result;
}

inline Quaternion from_euler_angles(Vector3 &euler_angles) {
    f32 cy = cos(euler_angles.yaw   * 0.5f);
    f32 sy = sin(euler_angles.yaw   * 0.5f);
    f32 cp = cos(euler_angles.pitch * 0.5f);
    f32 sp = sin(euler_angles.pitch * 0.5f);
    f32 cr = cos(euler_angles.roll  * 0.5f);
    f32 sr = sin(euler_angles.roll  * 0.5f);

    Quaternion q;
    q.w = cr * cp * cy + sr * sp * sy;
    q.x = sr * cp * cy - cr * sp * sy;
    q.y = cr * sp * cy + sr * cp * sy;
    q.z = cr * cp * sy - sr * sp * cy;

    return q;
}

inline Vector3 to_euler_angles(Quaternion &q) {
    Vector3 result = {};

    // roll (x-axis rotation)
    f32 sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
    f32 cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
    result.roll = atan2(sinr_cosp, cosr_cosp);

    // pitch (y-axis rotation)
    f32 sinp = 2 * (q.w * q.y - q.z * q.x);
    if (abs(sinp) >= 1)
        result.pitch = copysign(PI / 2.0f, sinp); // use 90 degrees if out of range
    else
        result.pitch = asin(sinp);

    // yaw (z-axis rotation)
    f32 siny_cosp = 2 * (q.w * q.z + q.x * q.y);
    f32 cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
    result.yaw = atan2(siny_cosp, cosy_cosp);

    return result;
}

inline Matrix4x4 to_rotation_matrix(Quaternion &q) {
    f32 x2 = q.x * q.x;
    f32 y2 = q.y * q.y;
    f32 z2 = q.z * q.z;

    f32 xy = q.x * q.y;
    f32 xz = q.x * q.z;
    f32 yz = q.y * q.z;

    f32 wx = q.w * q.x;
    f32 wy = q.w * q.y;
    f32 wz = q.w * q.z;

    Matrix4x4 result = {};

    result.table[0][0] = 1.0f - 2.0f * (y2 + z2);
    result.table[0][1] = 2.0f * (xy - wz);
    result.table[0][2] = 2.0f * (xz + wy);
    result.table[0][3] = 0.0f;

    result.table[1][0] = 2.0f * (xy + wz);
    result.table[1][1] = 1.0f - 2.0f * (x2 + z2);
    result.table[1][2] = 2.0f * (yz - wx);
    result.table[1][3] = 0.0f;

    result.table[2][0] = 2.0f * (xz - wy);
    result.table[2][1] = 2.0f * (yz + wx);
    result.table[2][2] = 1.0f - 2.0f * (x2 + y2);
    result.table[2][3] = 0.0f;

    result.table[3][0] = 0.0f;
    result.table[3][1] = 0.0f;
    result.table[3][2] = 0.0f;
    result.table[3][3] = 1.0f;

    return result;
}

#endif
