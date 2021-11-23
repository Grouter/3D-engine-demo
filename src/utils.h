inline u64 millis() {
    timespec t;

    timespec_get(&t, TIME_UTC);

    return (t.tv_sec * 1000) + (t.tv_nsec / 1000000);
}

internal char* copy_string(const char *src, u64 length) {
    char *result = (char *)calloc(length + 1, 1);
    strncpy(result, src, length);

    return result;
}

internal char* copy_string(const char *src) {
    u64 length = strlen(src);

    return copy_string(src, length);
}
