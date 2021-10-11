#if !defined(CAMERA_H)
#define CAMERA_H

#define CAMERA_SENS 0.1f

struct Camera {
    f32 fov;
    f32 clip_near;
    f32 clip_far;

    Vector3 position;
    Vector3 rotation;

    Matrix4x4 perspective;
    Matrix4x4 transform;
};

#endif
