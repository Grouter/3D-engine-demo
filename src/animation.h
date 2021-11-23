#if !defined(ANIMATION_H)
#define ANIMATION_H

struct CameraAnimation {
    u32 key_count = 0;
    Array<Vector3> positions;
    Array<Vector3> rotations;
};

internal CameraAnimation load_camera_animation(const char *path) {
    CameraAnimation result = {};

    Array<char> file;
    {
        bool read_result = read_whole_file(path, file);

        if (!read_result) {
            log_print("No such keyframe file %s\n", path);
            result.key_count = 0;
            return result;
        }
    }

    i32 key_index = -1; // -1 so the first '!' will increment it to 0
    char *walker = file.data;

    // Count key count so we can allocate the memory upfront
    while (*walker) {
        if (*walker == '!') {
            result.key_count += 1;

            walker = eat(walker, '\n', true);
        }
        else if (*walker == '#') {
            walker = eat(walker, '\n', true);
        }

        walker += 1;
    }

    if (result.key_count == 0) {
        log_print("0 keys were found in %s\n", path);
        return result;
    }

    allocate_array(result.positions, result.key_count);
    allocate_array(result.rotations, result.key_count);

    // Rewind the walker back to the start
    walker = file.data;

    while (*walker) {
        walker = eat_whitespace(walker);

        if (*walker == '!') {
            key_index += 1;

            walker += 1;
            continue;
        }

        if (strncmp(walker, "position", 8) == 0) {
            Vector3 pos;
            sscanf(walker, "position: %f %f %f", &pos.x, &pos.y, &pos.z);

            if (result.positions.length <= key_index) result.positions.add(pos);
            else result.positions[key_index] = pos;
        }
        else if (strncmp(walker, "rotation", 8) == 0) {
            Vector3 rot;
            sscanf(walker, "rotation: %f %f %f", &rot.x, &rot.y, &rot.z);

            if (result.rotations.length <= key_index) result.rotations.add(rot);
            else result.rotations[key_index] = rot;
        }

        walker = eat(walker, '\n', true);
        walker = eat_whitespace(walker);
    }

    free_array(file);

    return result;
}

#endif
