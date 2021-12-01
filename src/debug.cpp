internal void draw_debug_info(f32 dt) {
    Font &font = get_font(FontResource_Small);

    f32 line_offset = font.size * game_state.pixels_to_units_2d * 1.2f;

    Vector3 position = {};
    position.y = (f32)game_state.viewport.height * game_state.pixels_to_units_2d;
    position.y -= FONT_SIZE_SMALL * game_state.pixels_to_units_2d;
    position.z = 0.0f;

    const u64 DEBUG_TEXT_BUFFER_SIZE = 64;
    static char debug_text_buffer[DEBUG_TEXT_BUFFER_SIZE];

    Color debug_color = make_color(0, 0, 0);

    // FPS
    {
        snprintf(debug_text_buffer, DEBUG_TEXT_BUFFER_SIZE, "FPS: %.2f", 1.0f / dt);

        draw_text(font, debug_text_buffer, position, debug_color);

        position.y -= line_offset;
    }

    // Entity count
    {
        snprintf(
            debug_text_buffer,
            DEBUG_TEXT_BUFFER_SIZE,
            "Rendering %d entities",
            game_state.entities.base_entities.stored
        );

        draw_text(font, debug_text_buffer, position, debug_color);

        position.y -= line_offset;
    }
}