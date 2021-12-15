internal void command_ping(Array<char *> &args) {
    console_add_to_history("Pong!");
}

internal void command_add(Array<char *> &args) {
    i32 a, b;
    command_parse_args2(args, &a, &b);

    console_add_to_history("%d + %d = %d", a, b, a + b);
}

internal void command_start_camera(Array<char *> &args) {
    console_add_to_history("Starting camera animation...");

    char path[64];
    snprintf(path, 64, "%s.keyframes", args[1]);

    load_camera_animation(path, game_state.resources.camera_animation);

    camera_start_animation(game_state.camera);
}

internal void command_start_camera_at(Array<char *> &args) {
    camera_start_animation(game_state.camera);

    i32 index = 0;
    command_parse_args1(args, &index);

    console_add_to_history("Starting camera animation at %d", index);

    game_state.camera.animation_key_index = index;
}

internal void command_camera_position(Array<char *> &args) {
    console_add_to_history("Camera stats:");
    console_add_to_history("    Position: %f %f %f", game_state.camera.position.x, game_state.camera.position.y, game_state.camera.position.z);
    console_add_to_history("    Rotation: %f %f %f", game_state.camera.rotation.x, game_state.camera.rotation.y, game_state.camera.rotation.z);
}