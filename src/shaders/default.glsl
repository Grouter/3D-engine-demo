#version 330

#ifdef VERTEX

in vec3 position;
in vec3 color;

uniform mat4 transform;

// @Todo: we can pass these as one
uniform mat4 projection;
uniform mat4 view;

out vec3 v_color;

void main() {
    v_color = color;

    gl_Position = projection * view * transform * vec4(position, 1.0);
}

#endif

#ifdef FRAGMENT

in vec3 v_color;

out vec4 color;

void main() {
    color = vec4(v_color, 1.0);
}

#endif
