#if !defined(CAMERA_H)
#define CAMERA_H

const f32 CAMERA_SENS = 1.0f;

const f32 CAMERA_SPEED = 0.05f;
const f32 CAMERA_DRAG = 0.98f;
const f32 CAMERA_MAX_SPEED = 0.008f;

const f32 CAMERA_ANGULAR_DRAG = 0.98f;
const f32 CAMERA_MAX_ANGULAR_SPEED = 20.0f;

struct Camera {
    f32 fov;
    f32 clip_near;
    f32 clip_far;

    Vector3 position;
    Vector3 velocity;
    Vector3 rotation;
    Vector3 rotation_velocity;
    Vector3 direction;

    Matrix4x4 perspective;
    Matrix4x4 transform;

    bool32 animation_play = false;
    f32 animation_speed = 1.0f;
    u32 animation_key_index;
    f32 animation_t;
};

#endif
