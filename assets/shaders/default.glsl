#ifdef VERTEX

in vec3 position;

void main() {
    //gl_Position = projection * view * transform * vec4(position, 1.0);
    gl_Position = vec4(position, 1.0);
}

#endif

#ifdef FRAGMENT

out vec4 fragment_color;

void main() {
    fragment_color = vec4(0.0, 0.0, 0.0, 1.0);
}

#endif
