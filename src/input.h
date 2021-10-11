#if !defined(INPUT_H)
#define INPUT_H

struct KeyInput {
    u8       scan_code;
    u16      virtual_code;
    wchar_t  character;
    bool32   alt_down;  // @Todo: bitfield with other modifiers
    bool32   pressed;
};

struct InputState {
    bool32 mouse_locked;

    i32 mouse_x;
    i32 mouse_y;

    i32 mouse_dx;
    i32 mouse_dy;

    i32 mouse_old_x;
    i32 mouse_old_y;
};

#endif
