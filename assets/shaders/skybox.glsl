#ifdef VERTEX

// Vertex data
in vec3 position;

uniform mat4 projection;
uniform mat4 view;

out vec3 f_uv;

void main() {
    f_uv = position;

    vec4 pos = projection * view * vec4(position, 1.0);
    gl_Position = pos.xyww;
}

#endif

#ifdef FRAGMENT

in vec3 f_uv;

uniform samplerCube skybox;

out vec4 fragment_color;

void main() {
    vec3 uv = f_uv;
    uv.x *= -1.0;

    fragment_color = texture(skybox, uv);
}

#endif