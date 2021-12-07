// #define CEL_SHADING

#ifdef VERTEX

in vec3 position;
in vec3 normal;
in vec2 uv;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model; // @Temporary

out vec3 f_frag_pos;
out vec3 f_normal;
out vec2 f_uv;

void main() {
    f_frag_pos = vec3(model * vec4(position, 1.0));
    f_normal = normal;
    f_uv = uv;

    gl_Position = projection * view * model * vec4(position, 1.0);
}

#endif

#ifdef FRAGMENT

const vec3 SUN_COLOR = vec3(0.349, 0.188, 0.360);
const float AMBIENT_STRENGTH = 0.2;
const float SPECULAR = 0.5;

in vec3 f_frag_pos;
in vec3 f_normal;
in vec2 f_uv;

uniform mat4 view;
uniform vec3 sun_dir;
uniform vec4 material_color;
uniform sampler2D diffuse_texture;

uniform sampler2DArray shadow_textures;

layout(std140, binding = 0) uniform LightMatricies {
    mat4 lights[CASCADE_COUNT];
};
uniform float cascade_distances[CASCADE_COUNT];

out vec4 fragment_color;

float calc_shadow(int layer) {
    vec4 light_frag_pos = lights[layer] * vec4(f_frag_pos, 1.0);
    vec3 light_lookup = light_frag_pos.xyz / light_frag_pos.w;
    light_lookup = light_lookup * 0.5 + 0.5;

    float current_depth = light_lookup.z;

    if (current_depth > 1.0) return 0.0;

    float bias = max(0.00001 * (1.0 - dot(f_normal, sun_dir)), 0.000001);

    float result = 0.0;
#if 0
    float closest_depth = texture(shadow_textures, vec3(light_lookup.xy, layer)).r;
    result = (current_depth - bias) > closest_depth ? 1.0 : 0.0;
#else
    vec2 texel_size = 1.0 / vec2(textureSize(shadow_textures, 0));

    int soft_count = 2;

    for (int x = -soft_count; x <= soft_count; ++x) {
        for (int y = -soft_count; y <= soft_count; ++y) {
            float closest_depth = texture(shadow_textures, vec3(light_lookup.xy + vec2(x, y) * texel_size, layer)).r;
            result += (current_depth - bias) > closest_depth ? 1.0 : 0.0;
        }
    }

    float s = (soft_count * 2.0 + 1.0);
    result /= (s * s);
#endif
    return result;
}

void main() {
    vec3 camera_position = -1.0 * vec3(view[3][0], view[3][1], view[3][2]);
    vec3 camera_dir = -1.0 * vec3(view[0][2], view[1][2], view[2][2]);
    vec3 camera_to_frag = normalize(camera_position - f_frag_pos);

    // Specular
    vec3 reflect_dir = -reflect(sun_dir, f_normal);
    float specular_intensity = pow(max(dot(reflect_dir, camera_dir), 0.0), 64);
#ifdef CEL_SHADING
    specular_intensity = step(0.5, specular_intensity);
    if (specular_intensity >= 0.8) specular_intensity = 1.0;
    else if (specular_intensity >= 0.6) specular_intensity = 0.6;
    else if (specular_intensity >= 0.3) specular_intensity = 0.3;
    else specular_intensity = 0.0;
#endif
    vec3 specular = SPECULAR * specular_intensity * SUN_COLOR;

    // Ambient
    vec3 ambient = AMBIENT_STRENGTH * SUN_COLOR;
    vec3 sun_dir_n = normalize(sun_dir);

    // Diffuse
    float diffuse_intensity = max(dot(f_normal, -sun_dir), 0.0);
#ifdef CEL_SHADING
    diffuse_intensity = step(0.1, diffuse_intensity);
    if (diffuse_intensity >= 0.8) diffuse_intensity = 1.0;
    else if (diffuse_intensity >= 0.6) diffuse_intensity = 0.6;
    else if (diffuse_intensity >= 0.3) diffuse_intensity = 0.3;
    else diffuse_intensity = 0.0;
#endif
    vec3 diffuse = diffuse_intensity * SUN_COLOR;

    // Shadows
    int layer = -1;

    {
        vec4 light_frag_pos_view_space = view * vec4(f_frag_pos, 1.0);
        float depth = abs(light_frag_pos_view_space.z);

        for (int i = 0; i < CASCADE_COUNT; i++) {
            if (depth < cascade_distances[i]) {
                layer = i;
                break;
            }
        }

        if (layer == -1) layer = CASCADE_COUNT - 1;
    }

    float shadow = calc_shadow(layer);

    // Results
    vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * material_color.rgb;

    vec4 texture_sample = texture(diffuse_texture, f_uv);

#if 0
    if (layer == 0) texture_sample = vec4(1.0, 0.0, 0.0, 1.0);
    else if (layer == 1) texture_sample = vec4(0.0, 1.0, 0.0, 1.0);
    else if (layer == 2) texture_sample = vec4(0.0, 0.0, 1.0, 1.0);
#endif

    fragment_color = vec4(result, 1.0) * texture_sample;
}

#endif
