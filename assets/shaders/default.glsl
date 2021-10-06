#ifdef VERTEX

in vec3 position;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model; // @Temporary

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0);
}

#endif

#ifdef FRAGMENT

out vec4 fragment_color;

void main() {
    fragment_color = vec4(0.0, 0.0, 0.0, 1.0);
}

#endif