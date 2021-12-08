#if !defined(PARSER_H)
#define PARSER_H

enum VariableType {
    VariableType_INTEGER,
    VariableType_FLOAT,
    VariableType_STRING,
    VariableType_VECTOR3,
    VariableType_VECTOR4,
};

struct RValue {
    VariableType type;

    union {
        i32 integer_value;
        f32 float_value;
        char *string_value;
        Vector3 vector3_value;
        Vector4 vector4_value;
    };
};

struct VariableBinding {
    char *name;
    RValue value;
};

internal bool parse_and_check_rvalue(char *buffer, RValue &value) {
    buffer = eat_spaces(buffer);

    u32 comma_count = 0;
    u32 quotes_count = 0;
    bool is_float = false;

    {
        char *walker = buffer;
        while (*walker != '\0' && *walker != '\n') {
            walker = eat_spaces(walker);

            // It is a string!
            if (*walker == '"') {
                quotes_count += 1;
                walker += 1;

                walker = eat(walker, '"', false);
                if (*walker == '"') {
                    walker += 1;
                    quotes_count += 1;
                    break;
                }
                else {
                    log_print("Invalid string rvalue!\n");
                    return false;
                }
            }

            if (*walker == '-') walker++;

            while (isdigit(*walker)) walker++;
            walker = eat_spaces(walker);

            if (*walker == ',') {
                comma_count += 1;
                walker += 1;
            }
            else if (*walker == '.') {
                is_float = true;
                walker += 1;
            }
        }
    }

    i32 scanned = -1;

    if (quotes_count == 2) {
        static char rvalue_string_buffer[64];
        sscanf(buffer, "\"%[^\"]\"", rvalue_string_buffer);

        value.type = VariableType_STRING;
        value.string_value = copy_string(rvalue_string_buffer);

        return true;
    }
    else if (is_float) {
        switch (comma_count) {
            case 0 : {
                value.type = VariableType_FLOAT;
                scanned = sscanf(buffer, "%f", &value.float_value);
            } break;

            case 2 : {
                value.type = VariableType_VECTOR3;
                scanned = sscanf(buffer, "%f, %f, %f", &value.vector3_value.x, &value.vector3_value.y, &value.vector3_value.z);
            } break;

            case 3 : {
                value.type = VariableType_VECTOR4;
                scanned = sscanf(buffer, "%f, %f, %f, %f", &value.vector4_value.x, &value.vector4_value.y, &value.vector4_value.z, &value.vector4_value.w);
            } break;

            default : return false;
        }
    }
    else {
        switch (comma_count) {
            case 0 : {
                value.type = VariableType_INTEGER;
                scanned = sscanf(buffer, "%d", &value.integer_value);
            } break;

            default : return false;
        }
    }

    return (scanned == (i32)(comma_count + 1));
}

internal char* parse_file_entry(Array<VariableBinding> &bindings, char** entry_type, char *buffer) {
    // Parse entry header
    {
        buffer = eat(buffer, '!', true);
        buffer = eat_spaces(buffer);

        if (*buffer == '\0') return buffer;

        *entry_type = buffer;
        buffer = eat(buffer, '\n', true);
    }

    // Parse values
    while (*buffer) {
        buffer = eat_whitespace(buffer);

        if (*buffer == '!') {
            return buffer;
        }

        if (*buffer == '#') {
            buffer = eat(buffer, '\n', true);
            continue;
        }

        if (*buffer == '\0') {
            return buffer;
        }


        VariableBinding *b = bindings.allocate();

        // Name
        {
            b->name = buffer;

            buffer = eat_until_whitespace(buffer);
            if (*(buffer - 1) != ':') {
                log_print("Invalid variable name declaration!\n");

                bindings.remove_last();

                buffer = eat(buffer, '\n', true);

                continue;
            }

            u64 name_length = buffer - b->name - 1; // -1 for ':' char
            char *name = (char *)calloc(name_length + 1, sizeof(char)); // +1 for '\0'

            memcpy(name, b->name, name_length);
            b->name = name;
        }

        bool is_valid = parse_and_check_rvalue(buffer, b->value);

        if (!is_valid) {
            log_print("Invalid value/type %s\n", b->name);

            free(b->name);
            bindings.remove_last();
        }

        buffer = eat(buffer, '\n', true);
    }

    return buffer;
}

#endif
