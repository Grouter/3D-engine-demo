#if !defined(CONSOLE_H)
#define CONSOLE_H

const f32 CONSOLE_OPEN_SPEED = 2.0f;

struct Console {
    TextInput input;
    f32 open_target = 0.0f;
    f32 openness    = 0.0f;
    Array<char *> history;
};

global bool console_open = false;
global Console _console;

internal void console_add_to_history(const char* format, ...);

#endif
