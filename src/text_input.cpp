internal void draw_text_input(TextInput &input, f32 dt, Vector3 bottom_left, Vector2 size, Font &font, f32 font_scale, Color text_color = Color_BLACK) {
    f32 font_unit_height = font.size * game_state.pixels_to_units * font_scale;

    // Draw text
    {
        Vector3 position = make_vector3(
            bottom_left.x,
            bottom_left.y + (size.y * 0.5f) - (font_unit_height * 0.5f),
            bottom_left.z
        );

        draw_text(font, input.contents.data, position, text_color, font_scale);
    }

    // Draw cursor
    if ((input.last_input_time < TEXT_INPUT_BLINK_DELAY) || (fmod(input.last_input_time, 1.0f) <= 0.5f)) {
        f32 text_width = get_string_width_in_units(font, input.contents.data, font_scale);
        f32 cursor_width = 0.1f;
        f32 cursor_offset = 0.02f;

        Vector2 cursor_size = make_vector2(
            cursor_width,
            font_unit_height
        );

        Vector3 position = make_vector3(
            bottom_left.x + text_width + cursor_width * 0.5f + cursor_offset,
            bottom_left.y + (size.y * 0.5f),
            bottom_left.z
        );

        draw_rect(position, cursor_size, Color_WHITE);
    }

    input.last_input_time += dt;
}

internal void text_input_handle_char(TextInput &input, KeyInput &key) {
    if (key.character == VK_BACK) {
        input.delete_last_char();
    }
    else if (key.character >= 32 && key.character <= 126) {
        input.add_char((char)key.character);
    }
}
