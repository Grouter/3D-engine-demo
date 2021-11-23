internal void command_ping(Array<char *> &args) {
    console_add_to_history("Pong!");
}

internal void command_add(Array<char *> &args) {
    i32 a, b;
    command_parse_args2(args, &a, &b);

    console_add_to_history("%d + %d = %d", a, b, a + b);
}
