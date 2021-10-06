#include "platform.h"
#include "camera.h"

inline void update_perspective(Camera &camera, u32 window_w, u32 window_h) {
    camera.perspective = perspective(camera.fov, camera.clip_near, camera.clip_far, (f32)window_w, (f32)window_h);

    //camera.perspective = orto(-1.0, 1.0, (f32)window_w, (f32)window_h);
}

internal Camera create_camera(u32 window_w, u32 window_h, f32 fov) {
    Camera camera = {};

    camera.fov = fov;
    camera.clip_near = 0.01f;
    camera.clip_far  = 1000.0f;

    update_perspective(camera, window_w, window_h);

    return camera;
}