#if !defined(INPUT_H)
#define INPUT_H

struct InputState {
    bool mouse_locked;

    i32 mouse_x;
    i32 mouse_y;

    i32 mouse_dx;
    i32 mouse_dy;

    i32 mouse_old_x;
    i32 mouse_old_y;
};

#endif
