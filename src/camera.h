#if !defined(CAMERA_H)
#define CAMERA_H

#define CAMERA_SENS 0.1f

const f32 CAMERA_SPEED = 0.2f;
const f32 CAMERA_DRAG = 0.9f;
const f32 CAMERA_MAX_SPEED = 5.0f;

struct Camera {
    f32 fov;
    f32 clip_near;
    f32 clip_far;

    Vector3 position;
    Vector3 velocity;
    Vector3 rotation;
    Vector3 direction;

    Matrix4x4 perspective;
    Matrix4x4 transform;

    bool32 animation_play = false;
    f32 animation_speed = 1.0f;
    u32 animation_key_index;
    f32 animation_t;
};

#endif
