#if !defined(ANIMATION_H)
#define ANIMATION_H

struct CameraAnimation {
    u32 key_count = 0;
    Array<Vector3> positions = {};
    Array<Vector3> rotations = {};
    Array<f32> times = {};
};

internal void load_camera_animation(const char *path, CameraAnimation &animation) {
    if (animation.positions.data) free_array(animation.positions);
    if (animation.rotations.data) free_array(animation.rotations);
    if (animation.times.data) free_array(animation.times);

    animation = {};

    Array<char> file;
    {
        bool read_result = read_whole_file(path, file);

        if (!read_result) {
            log_print("No such keyframe file %s\n", path);
            animation.key_count = 0;
            return;
        }
    }

    i32 key_index = -1; // -1 so the first '!' will increment it to 0
    char *walker = file.data;

    // Count key count so we can allocate the memory upfront
    while (*walker) {
        if (*walker == '!') {
            animation.key_count += 1;

            walker = eat(walker, '\n', true);
        }
        else if (*walker == '#') {
            walker = eat(walker, '\n', true);
        }

        walker += 1;
    }

    if (animation.key_count == 0) {
        log_print("0 keys were found in %s\n", path);
        return;
    }

    allocate_array(animation.positions, animation.key_count);
    allocate_array(animation.rotations, animation.key_count);
    allocate_array(animation.times, animation.key_count);

    Vector3 position = {};
    Vector3 rotation = {};

    // Rewind the walker back to the start
    walker = file.data;

    while (*walker) {
        walker = eat_whitespace(walker);

        if (*walker == '!') {
            key_index += 1;

            animation.positions.add(position);
            animation.rotations.add(rotation);
            animation.times.add(1.0f);

            walker += 1;
            continue;
        }

        if (strncmp(walker, "position", 8) == 0) {
            sscanf(walker, "position: %f, %f, %f", &position.x, &position.y, &position.z);
            animation.positions[key_index] = position;
        }
        else if (strncmp(walker, "rotation", 8) == 0) {
            sscanf(walker, "rotation: %f, %f, %f", &rotation.x, &rotation.y, &rotation.z);
            animation.rotations[key_index] = rotation;
        }
        else if (strncmp(walker, "time", 4) == 0) {
            f32 time;
            sscanf(walker, "time: %f", &time);

            animation.times[key_index] = time;
        }

        walker = eat(walker, '\n', true);
        walker = eat_whitespace(walker);
    }

    free_array(file);
}

#endif
