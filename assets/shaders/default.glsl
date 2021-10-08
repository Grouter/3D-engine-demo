#ifdef VERTEX

in vec3 position;
in vec3 normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model; // @Temporary

out vec3 c;

void main() {
    c = normal;

    gl_Position = projection * view * model * vec4(position, 1.0);
}

#endif

#ifdef FRAGMENT

in vec3 c;

out vec4 fragment_color;

void main() {
    fragment_color = vec4(c, 1.0);
}

#endif