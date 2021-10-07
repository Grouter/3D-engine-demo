internal void handle_key_down(u8 scan_code, u16 virtual_code, bool alt_down) {
    const f32 CAMERA_SPEED = 10.0f;

    if (virtual_code == VK_LEFT) {
        game_state.camera.position.x += CAMERA_SPEED;
    }
    else if (virtual_code == VK_RIGHT) {
        game_state.camera.position.x -= CAMERA_SPEED;
    }
    else if (virtual_code == VK_UP) {
        game_state.camera.position.y += CAMERA_SPEED;
    }
    else if (virtual_code == VK_DOWN) {
        game_state.camera.position.y -= CAMERA_SPEED;
    }
    else if (virtual_code == VK_HOME) {
        game_state.camera.position.z += CAMERA_SPEED;
    }
    else if (virtual_code == VK_END) {
        game_state.camera.position.z -= CAMERA_SPEED;
    }
}

internal void handle_key_up(u8 scan_code, u16 virtual_code, bool alt_down) {

}

internal void handle_char(wchar_t c) {

}

internal void handle_mouse_input() {

    input_state.mouse_dx = input_state.mouse_x - input_state.mouse_old_x;
    input_state.mouse_dy = input_state.mouse_y - input_state.mouse_old_y;

    input_state.mouse_old_x = input_state.mouse_x;
    input_state.mouse_old_y = input_state.mouse_y;
}