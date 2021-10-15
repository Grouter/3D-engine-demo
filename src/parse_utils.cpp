internal void read_whole_file(const char *name, Array<char> &buffer) {
    FILE *f = fopen(name, "r");

    if (!f) {
        log_print("Error opening file: %s\n", name);
        exit(1);
    }

    fseek(f, 0, SEEK_END);
    u64 size = ftell(f);
    fseek(f, 0, SEEK_SET);

    allocate_array(buffer, size + 1);

    fread(buffer.data, sizeof(char), size, f);
    buffer.data[size] = 0; // Zero terminate

    buffer.length = size + 1;

    fclose(f);
}

inline void eat_whitespace(char **buffer) {
    while ((*buffer)[0] > 0 && isspace((*buffer)[0]))
        *buffer = (*buffer) + 1;
}

inline void eat_until(char **buffer, char until) {
    while ((*buffer)[0] > 0 && (*buffer)[0] != until)
        *buffer = (*buffer) + 1;
}

inline u32 find(char *buffer, char to_find) {
    u32 res = 0;
    while (buffer[res] > 0 && buffer[res] != to_find) res++;
    return res;
}

inline u32 word_length(char *buffer) {
    u32 res = 0;
    while (buffer[res] > 0 && !isspace(buffer[res])) res++;
    return res;
}