#include "platform.h"
#include "camera.h"

inline void update_perspective(Camera &camera, u32 window_w, u32 window_h) {
    camera.perspective = perspective(camera.fov, camera.clip_near, camera.clip_far, (f32)window_w, (f32)window_h);
}

internal Camera create_camera(u32 window_w, u32 window_h, f32 fov) {
    Camera camera = {};

    camera.fov = fov;
    camera.clip_near = 0.01f;
    camera.clip_far  = 1000.0f;

    update_perspective(camera, window_w, window_h);

    return camera;
}

internal void camera_update(Camera &camera) {
    if (camera.rotation.pitch > 89.0f)
        camera.rotation.pitch = 89.0f;
    else if (camera.rotation.pitch < -89.0f)
        camera.rotation.pitch = -89.0f;

    Vector3 direction;
    direction.x = cos(TO_RADIANS(camera.rotation.yaw)) * cos(TO_RADIANS(camera.rotation.pitch));
    direction.y = sin(TO_RADIANS(camera.rotation.pitch));
    direction.z = sin(TO_RADIANS(camera.rotation.yaw)) * cos(TO_RADIANS(camera.rotation.pitch));

    Vector3 forward = normalized(direction);
    Vector3 side    = normalized(cross(V3_UP, forward));
    Vector3 up      = cross(forward, side);

    Vector3 position = make_vector3(
        -camera.position.x * side.x    - camera.position.y * side.y    - camera.position.z * side.z,
        -camera.position.x * up.x      - camera.position.y * up.y      - camera.position.z * up.z,
        -camera.position.x * forward.x - camera.position.y * forward.y - camera.position.z * forward.z
    );

    camera.transform = {
        side.x, up.x, forward.x, 0.0f,
        side.y, up.y, forward.y, 0.0f,
        side.z, up.z, forward.z, 0.0f,
        position.x, position.y, position.z, 1.0f,
    };
}

inline Vector3 get_forward_vector(Camera &camera) {
    return { camera.transform.raw[2], camera.transform.raw[6], camera.transform.raw[10] };
}

inline Vector3 get_up_vector(Camera &camera) {
    return { camera.transform.raw[1], camera.transform.raw[5], camera.transform.raw[9] };
}

inline Vector3 get_side_vector(Camera &camera) {
    return { camera.transform.raw[0], camera.transform.raw[4], camera.transform.raw[8] };
}

// @Speed: creating new vectors everytime
internal void camera_handle_input(Camera &camera, const KeyInput &input) {
    const f32 CAMERA_SPEED = 10.0f;

    Vector3 forward = get_forward_vector(camera);
    Vector3 side = get_side_vector(camera);

    if (input.virtual_code == VK_LEFT) {
        camera.position = add(camera.position, multiply(side, -CAMERA_SPEED));
    }
    else if (input.virtual_code == VK_RIGHT) {
        camera.position = add(camera.position, multiply(side, CAMERA_SPEED));
    }
    else if (input.virtual_code == VK_UP) {
        camera.position = add(camera.position, multiply(forward, -CAMERA_SPEED));
    }
    else if (input.virtual_code == VK_DOWN) {
        camera.position = add(camera.position, multiply(forward, CAMERA_SPEED));
    }
}

internal void camera_handle_mouse(Camera &camera, i32 dx, i32 dy) {
    camera.rotation.yaw   += (f32)dx * CAMERA_SENS;
    camera.rotation.pitch += (f32)dy * CAMERA_SENS;

    if (camera.rotation.yaw >= 360.0f) {
        camera.rotation.yaw -= 360.0f;
    }
    else if (camera.rotation.yaw <= -360.0f) {
        camera.rotation.yaw += 360.0f;
    }

    if (camera.rotation.pitch >= 360.0f) {
        camera.rotation.pitch -= 360.0f;
    }
    else if (camera.rotation.pitch <= -360.0f) {
        camera.rotation.pitch += 360.0f;
    }
}