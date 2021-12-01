#ifdef VERTEX

in vec3 position;
in vec3 normal;
in vec2 uv;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model; // @Temporary
uniform mat4 sun;

out vec3 f_frag_pos;
out vec4 f_light_frag_pos;
out vec3 f_normal;
out vec2 f_uv;

void main() {
    f_frag_pos = vec3(model * vec4(position, 1.0));
    f_light_frag_pos = sun * vec4(f_frag_pos, 1.0);
    f_normal = normal;
    f_uv = uv;

    gl_Position = projection * view * model * vec4(position, 1.0);
}

#endif

#ifdef FRAGMENT

const vec3  SUN_COLOR        = vec3(1.0, 0.945, 0.788);
const float AMBIENT_STRENGTH = 0.2;
const float SPECULAR = 0.5;

in vec3 f_frag_pos;
in vec4 f_light_frag_pos;
in vec3 f_normal;
in vec2 f_uv;

uniform vec3 sun_dir;
uniform vec3 camera_position;
uniform vec4 material_color;
uniform sampler2D diffuse_texture;
uniform sampler2D shadow_texture;

out vec4 fragment_color;

float calc_shadow(vec4 light_frag_pos) {
    vec3 light_lookup = light_frag_pos.xyz / light_frag_pos.w;
    light_lookup = light_lookup * 0.5 + 0.5;

    float closest_depth = texture(shadow_texture, light_lookup.xy).r;
    float current_depth = light_lookup.z;

#if 0
    float result = (current_depth - 0.005) > closest_depth ? 1.0 : 0.0;
#else
    float result = 0.0;
    vec2 texel_size = 1.0 / textureSize(shadow_texture, 0);

    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcf_depth = texture(shadow_texture, light_lookup.xy + vec2(x, y) * texel_size).r;
            result += current_depth - 0.005 > pcf_depth ? 1.0 : 0.0;
        }
    }
    result /= 9.0;
#endif
    return result;
}

void main() {
    vec3 view_dir = normalize(camera_position - f_frag_pos);
    vec3 reflect_dir = reflect(-sun_dir, f_normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
    vec3 specular = SPECULAR * spec * SUN_COLOR;

    vec3 ambient = AMBIENT_STRENGTH * SUN_COLOR;
    vec3 sun_dir_n = normalize(sun_dir);

    float diffuse_s = max(dot(f_normal, sun_dir), 0.0);
    vec3 diffuse = diffuse_s * SUN_COLOR;

    float shadow = calc_shadow(f_light_frag_pos);

    vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * material_color.rgb;

    vec4 texture_sample = texture(diffuse_texture, f_uv);

    fragment_color = vec4(result, 1.0) * texture_sample;
}

#endif