const f32 TEXT_INPUT_BLINK_DELAY = 0.5f;

struct TextInput {
    Array<char> contents;
    f32 last_input_time;

    inline void clear();
    inline void add_char(char c);
    inline void delete_last_char();
};

inline void init_text_input(TextInput &input, u32 size) {
    assert(size != 0);

    input.last_input_time = TEXT_INPUT_BLINK_DELAY;

    allocate_array(input.contents, size);
    memset(input.contents.data, 0, size * sizeof(char));
}

inline void TextInput::clear() {
    this->contents.clear();

    memset(this->contents.data, 0, this->contents.capacity * sizeof(char));
}

inline void TextInput::add_char(char c) {
    // +1 for zero terminated
    if ((this->contents.length + 1) < this->contents.capacity) {
        this->contents.add(c);
        this->last_input_time = 0.0f;
    }
}

inline void TextInput::delete_last_char() {
    if (this->contents.length > 0) {
        this->contents.remove_last_and_zero_out();
        this->last_input_time = 0.0f;
    }
}
