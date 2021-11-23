internal void init_console() {
    _console = {};
    init_text_input(_console.input, 64);
    allocate_array(_console.history, 32);
}

internal void console_add_to_history(const char* format, ...) {
    static char buffer[64];

    va_list args;
    va_start(args, format);
    _vsnprintf(buffer, 64, format, args);
    va_end(args);

    char* new_entry = copy_string(buffer);
    _console.history.add(new_entry);
}

internal void toggle_console() {
    console_open = !console_open;

    if (console_open) _console.open_target = 0.5f;
    else _console.open_target = 0.0f;
}

internal void draw_console(f32 dt) {
    if (!console_open) {
        if (_console.openness != _console.open_target) {
            _console.openness = max(0.0f, _console.openness - (CONSOLE_OPEN_SPEED * dt));
        }
        else return;
    }
    else {
        if (_console.openness != _console.open_target)
            _console.openness = min(_console.open_target, _console.openness + (CONSOLE_OPEN_SPEED * dt));
    }

    Font &font = get_font(FontResource_Medium);

    f32 screen_width = game_state.viewport.width * game_state.pixels_to_units_2d;
    f32 screen_height = game_state.viewport.height * game_state.pixels_to_units_2d;

    f32 text_scale = 0.6f;

    f32 input_height = font.size * text_scale * game_state.pixels_to_units_2d * 1.5f;
    f32 history_line_height = font.size * text_scale * game_state.pixels_to_units_2d * 1.5f;

    f32 text_left_padding = 0.05f;

    Vector2 background_size = make_vector2(screen_width, screen_height * _console.openness);

    Color text_color = make_color(222);

    // Draw BG
    {
        Vector3 position = make_vector3(
            screen_width * 0.5f,
            screen_height - (background_size.y * 0.5f),
            -0.02f
        );

        draw_rect(position, background_size, make_color(10, 90, 30));
    }

    // Draw input rect
    {
        Vector3 position = make_vector3(
            screen_width * 0.5f,
            screen_height - background_size.y + input_height * 0.5f,
            - 0.01f
        );

        Vector2 size = make_vector2(
            screen_width,
            input_height * 1.2f
        );

        draw_rect(position, size, make_color(70, 10, 10));
    }

    // Draw Input
    {
        Vector3 position = make_vector3(
            text_left_padding,
            screen_height - background_size.y,
            0.0f
        );

        Vector2 size = make_vector2(
            screen_width,
            input_height
        );

        draw_text_input(_console.input, dt, position, size, font, text_scale, text_color);
    }

    // Draw history
    {
        Vector3 position = make_vector3(
            text_left_padding,
            screen_height - background_size.y + input_height * 1.3f,
            0.0f
        );

        for (i32 i = (i32)_console.history.length - 1; i >= 0; i--) {
            draw_text(font, _console.history[i], position, text_color, text_scale);

            position.y += history_line_height;

            if (position.y > screen_height) break;
        }
    }
}

internal void console_handle_char_input(KeyInput &key) {
    if (key.character >= 32 && key.character <= 126) {
        _console.input.add_char((char)key.character);
    }
    else if (key.character == VK_BACK) {
        _console.input.delete_last_char();
    }
    else if (key.character == VK_RETURN) {
        if (_console.input.contents.length == 0) return;

        console_add_to_history("> %s", _console.input.contents.data);

        execute_command(_console.input.contents.data);

        _console.input.clear();
    }
}
