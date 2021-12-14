#ifdef VERTEX

// Vertex data
in vec3 position;
in vec2 uv;

out vec2 f_uv;

void main() {
    f_uv = uv;
    gl_Position = vec4(position, 1.0);
}

#endif

#ifdef FRAGMENT

in vec2 f_uv;

uniform sampler2D target_texture;

uniform int direction;

const float WEIGHTS[5] = float[] (0.22, 0.19, 0.12, 0.05, 0.01);

out vec4 fragment_color;

#ifdef DOWNSCALE
void main() {
    vec2 texel_size = 1.0 / textureSize(target_texture, 0).xy;

    vec3 result = vec3(0.0);

    texel_size *= 2.0;  // Sampling (higher -> less precise)

    result += texture(target_texture, f_uv - vec2(1.0, 0.0) * texel_size).xyz;
    result += texture(target_texture, f_uv + vec2(1.0, 0.0) * texel_size).xyz;
    result += texture(target_texture, f_uv - vec2(0.0, 1.0) * texel_size).xyz;
    result += texture(target_texture, f_uv + vec2(0.0, 1.0) * texel_size).xyz;
    result *= 0.25;

    fragment_color = vec4(result, 1.0);
}
#endif

#ifdef BLUR
void main() {
    vec2 texel_size = 1.0 / textureSize(target_texture, 0).xy;
    vec3 result = texture(target_texture, f_uv).rgb * WEIGHTS[0];

    if (direction == 0) {
        for(int i = 1; i < 5; i++) {
            result += texture(target_texture, f_uv + vec2(i, 0.0) * texel_size).rgb * WEIGHTS[i];
            result += texture(target_texture, f_uv - vec2(i, 0.0) * texel_size).rgb * WEIGHTS[i];
        }
    }
    else {
        for(int i = 1; i < 5; i++) {
            result += texture(target_texture, f_uv + vec2(0.0, i) * texel_size).rgb * WEIGHTS[i];
            result += texture(target_texture, f_uv - vec2(0.0, i) * texel_size).rgb * WEIGHTS[i];
        }
    }

    fragment_color = vec4(result, 1.0);
}
#endif

#endif