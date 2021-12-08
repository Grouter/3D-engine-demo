#ifdef VERTEX

// Vertex data
in vec3 position;
in vec2 uv;

// Instance data
in vec4 color;
in vec2 uv_offset;
in vec2 uv_scale;
in mat4 model;

uniform mat4 view;
uniform mat4 projection;

out vec4 f_color;
out vec2 f_uv;
out vec2 f_uv_offset;
out vec2 f_uv_scale;

void main() {
    f_uv = uv;
    f_color = color;
    f_uv_offset = uv_offset;
    f_uv_scale = uv_scale;

    gl_Position = projection * view * model * vec4(position, 1.0);
}

#endif

#ifdef FRAGMENT

in vec4 f_color;
in vec2 f_uv;
in vec2 f_uv_offset;
in vec2 f_uv_scale;

uniform sampler2D diffuse_texture;

out vec4 fragment_color;

void main() {
    // Recalculates the new UVs according to the uv_data
    vec2 mapped_uv;
    mapped_uv.x = f_uv_offset.x + (f_uv.x * f_uv_scale.x);
    mapped_uv.y = f_uv_offset.y + (f_uv.y * f_uv_scale.y);

    fragment_color = texture(diffuse_texture, mapped_uv) * f_color;
}

#endif