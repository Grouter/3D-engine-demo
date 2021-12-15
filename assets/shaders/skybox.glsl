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

layout (location = 0) out vec4 fragment_color;
layout (location = 1) out vec4 bloom_color;

void main() {
    vec3 uv = f_uv;
    uv.x *= -1.0;

    fragment_color = texture(skybox, uv);

    float brightness = max(fragment_color.r, max(fragment_color.g, fragment_color.b));
    if(brightness > 0.3) bloom_color = vec4(fragment_color.rgb * 0.2, 1.0);
    else bloom_color = vec4(0.0, 0.0, 0.0, 1.0);
    // bloom_color = vec4(0.0, 0.0, 0.0, 1.0);
}

#endif