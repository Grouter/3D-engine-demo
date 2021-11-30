#if !defined(COMMANDS_H)
#define COMMANDS_H

struct Command {
    char* name;
    u32   arg_count;
    void (*proc)(Array<char *> &);
};

global Array<Command> commands;

// Forward declare commands here:
internal void command_ping(Array<char *> &args);
internal void command_add(Array<char *> &args);
internal void command_start_camera(Array<char *> &args);

internal void _convert(const char *src, f32 *loc) { *loc = (f32)std::atof(src); }
internal void _convert(const char *src, i32 *loc) { *loc = std::atoi(src); }

#define command_parse_args1(tokens, loc1) { _convert((tokens)[1], loc1); }
#define command_parse_args2(tokens, loc1, loc2) { _convert((tokens)[1], loc1); _convert((tokens)[2], loc2); }

internal void add_command(const char *name, u32 arg_count, void (*proc)(Array<char *> &)) {
    Command *command = commands.allocate();
    command->name = copy_string(name);
    command->arg_count = arg_count;
    command->proc = proc;
}

internal void init_commands() {
    allocate_array(commands, 10);

    add_command("ping", 0, command_ping);
    add_command("add", 2, command_add);
    add_command("start_camera", 0, command_start_camera);
}

internal void execute_command(char *input) {
    Array<char *> tokens;
    allocate_array(tokens, 8);

    // Split string
    {
        char *walker = input;
        while (*walker) {
            walker = eat_spaces(walker);
            char *start = walker;
            walker = eat_until_whitespace(walker);
            u64 length = walker - start;

            tokens.add(copy_string(start, length));
        }
    }

    // It must always be at least one (command name)
    assert(tokens.length > 0);

    char *command_name = tokens[0];
    u32 arg_count = (u32)(tokens.length - 1);
    bool found = false;

    Command *it;
    array_foreach(commands, it) {
        if (strcmp(command_name, it->name) == 0) {
            found = true;

            if (arg_count != it->arg_count) {
                console_add_to_history("Invalid argument count: got %d expected %d", arg_count, it->arg_count);
                break;
            }

            it->proc(tokens);
        }
    }

    if (!found) {
        console_add_to_history("Command not found: %s", command_name);
    }

    for (i32 i = 0; i < tokens.length; i++) free(tokens[i]);
    free_array(tokens);
}

#endif
