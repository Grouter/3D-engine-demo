#if !defined(CAMERA_H)
#define CAMERA_H

#define SENS 0.1f

struct Camera {
    f32 fov;
    f32 clip_near;
    f32 clip_far;
    f32 yaw;
    f32 pitch;

    Vector3 position;

    Matrix4x4 perspective;
    Matrix4x4 transform;
};

#endif
