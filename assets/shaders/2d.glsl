#ifdef VERTEX

// Vertex data
in vec3 position;
in vec2 uv;

// Instance data
in vec4 color;
in vec2 uv_offset;
in vec2 uv_scale;
in mat4 model;

uniform mat4 projection;

out vec4 f_color;
out vec2 f_uv;
out vec2 f_uv_offset;
out vec2 f_uv_scale;

void main() {
    f_color = color;
    f_uv = uv;
    f_uv_offset = uv_offset;
    f_uv_scale = uv_scale;

    gl_Position = projection * model * vec4(position, 1.0);
}

#endif

#ifdef FRAGMENT

in vec4 f_color;
in vec2 f_uv;
in vec2 f_uv_offset;
in vec2 f_uv_scale;

uniform vec4 material_color;
uniform sampler2D diffuse_texture;
uniform bool diffuse_alpha_mask;

out vec4 fragment_color;

void main() {
    // Recalculates the new UVs according to the uv_data
    vec2 mapped_uv;
    mapped_uv.x = f_uv_offset.x + (f_uv.x * f_uv_scale.x);
    mapped_uv.y = f_uv_offset.y + (f_uv.y * f_uv_scale.y);

    vec4 texture_sample = texture2D(diffuse_texture, mapped_uv);

    if (diffuse_alpha_mask) {
        fragment_color = f_color;
        fragment_color.a *= texture_sample.r;
    }
    else {
        fragment_color = f_color;
    }

    if (fragment_color.a == 0) discard;
}

#endif