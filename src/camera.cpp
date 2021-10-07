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

internal void camera_update(Camera &camera, InputState &input_state) {
    
    camera.yaw += (f32)input_state.mouse_dx * SENS;
    camera.pitch += (f32)input_state.mouse_dy * SENS;

    if (camera.pitch > 89.0f)
        camera.pitch = 89.0f;
    
    if (camera.pitch < -89.0f)
        camera.pitch = -89.f;

    Vector3 direction;

    direction.x = cos(TO_RADIANS(camera.yaw)) * cos(TO_RADIANS(camera.pitch));
    direction.y = sin(TO_RADIANS(camera.pitch));
    direction.z = sin(TO_RADIANS(camera.yaw)) * cos(TO_RADIANS(camera.pitch));

    Vector3 global_up = make_vector3(0.0f, 1.0f, 0.0f);

    Vector3 forward = normalized(direction);;

    Vector3 side = cross(global_up, forward); 
    Vector3 n_side = normalized(side);

    Vector3 up = cross(forward, n_side);

    Vector3 position = {
        -camera.position.x * n_side.x    - camera.position.y * n_side.y   - camera.position.z * n_side.z,
        -camera.position.x * up.x        - camera.position.y * up.y       - camera.position.z * up.z,
        -camera.position.x * forward.x   - camera.position.y * forward.y  - camera.position.z * forward.z
    };

    camera.transform = {
        n_side.x, up.x, forward.x, 0.0f,
        n_side.y, up.y, forward.y, 0.0f,
        n_side.z, up.z, forward.z, 0.0f,
        position.x, position.y, position.z, 1.0f,
    };
}