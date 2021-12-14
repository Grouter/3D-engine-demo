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

uniform sampler2D hdr_buffer;
uniform sampler2D bloom_buffer;

const float BLOOM_AMOUNT = 1.0;
const float EXPOSURE = 1.2;
const float GAMMA = 2.2;

out vec4 fragment_color;

const float A = 0.15;
const float B = 0.50;
const float C = 0.10;
const float D = 0.20;
const float E = 0.02;
const float F = 0.30;
const float W = 11.2;

vec3 tone_map(vec3 x) {
    return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

void main() {
    vec3 hdr_color = texture(hdr_buffer, f_uv).rgb;
    vec3 bloom_color = texture(bloom_buffer, f_uv).rgb;

    hdr_color += bloom_color * BLOOM_AMOUNT;

#if 0
    vec3 result = vec3(1.0) - exp(-hdr_color * EXPOSURE);
    result = pow(hdr_color, vec3(1.0 / GAMMA));
#else
    hdr_color *= EXPOSURE;

    vec3 curr = tone_map(hdr_color * 2.0);
    vec3 white_scale = vec3(1.0) / tone_map(vec3(W));
    vec3 color = curr * white_scale;
    vec3 result = pow(color, vec3(1.0 / GAMMA));
#endif
    fragment_color = vec4(result, 1.0);
}

#endif