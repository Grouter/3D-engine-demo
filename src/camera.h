#if !defined(CAMERA_H)
#define CAMERA_H

struct Camera {
    f32 fov;
    f32 clip_near;
    f32 clip_far;

    Vector3 position;

    Matrix4x4 perspective;
};

#endif
