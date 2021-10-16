#if !defined(HOTLOAD_H)
#define HOTLOAD_H

enum HotloadFileType {
    Shaders,
};

struct HotloadBind {
    void (*callback)(const char *, u8);
};

struct HotloadProcessEntry {
    char file_name[32];
    u8   file_name_len;
    HotloadFileType type;
};

#endif
