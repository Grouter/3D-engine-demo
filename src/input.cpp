internal void toggle_cursor_state() {
    input_state.mouse_locked = !input_state.mouse_locked;

    ShowCursor(!input_state.mouse_locked);
}

internal void handle_key_down(u8 scan_code, u16 virtual_code, bool alt_down) {
    KeyInput key = {};
    key.scan_code    = scan_code;
    key.virtual_code = virtual_code;
    key.alt_down     = alt_down;
    key.pressed      = false;

    camera_handle_input(game_state.camera, key);

    if (virtual_code == VK_ESCAPE) {
        toggle_cursor_state();
    }
}

internal void handle_key_up(u8 scan_code, u16 virtual_code, bool alt_down) {
    KeyInput key = {};
    key.scan_code    = scan_code;
    key.virtual_code = virtual_code;
    key.alt_down     = alt_down;
    key.pressed      = false;
}

internal void handle_char(wchar_t c) {
    KeyInput key = {};
    key.character = c;
    key.pressed   = true;
}

internal void handle_mouse_input() {
    input_state.mouse_dx = input_state.mouse_x - input_state.mouse_old_x;
    input_state.mouse_dy = input_state.mouse_y - input_state.mouse_old_y;

    input_state.mouse_old_x = input_state.mouse_x;
    input_state.mouse_old_y = input_state.mouse_y;

    if (input_state.mouse_locked) {
        camera_handle_mouse(game_state.camera, input_state.mouse_dx, input_state.mouse_dy);
    }
}