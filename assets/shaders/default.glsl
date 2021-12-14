// #define CEL_SHADING

#ifdef VERTEX

// Vertex data
in vec3 position;
in vec3 normal;
in vec2 uv;

#ifdef GRASS_SHADER
// Instance data
in mat4 model;

uniform mat4 projection;
uniform mat4 view;
#else
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model; // @Temporary
#endif

uniform float time;

out float f_mesh_y;
out vec3 f_frag_pos;
out vec3 f_normal;
out vec2 f_uv;

void main() {
    f_mesh_y = position.y;
    f_frag_pos = vec3(model * vec4(position, 1.0));
    f_normal = normal;
    f_uv = uv;

    vec4 pos = vec4(position, 1.0);

#ifdef TREE_SHADER
    if (pos.y >= 0.5) pos.x = position.x + sin(time) * 0.02 * pos.y;
#endif

#ifdef GRASS_SHADER
    if (pos.y >= 0.2) pos.x = position.x + sin(time) * 0.08 * pos.y;
#endif

    gl_Position = projection * view * model * pos;
}

#endif

#ifdef FRAGMENT

const vec3 SUN_COLOR = vec3(0.349, 0.188, 0.360);
const float AMBIENT_STRENGTH = 0.2;
const float SPECULAR = 0.5;

in float f_mesh_y;
in vec3 f_frag_pos;
in vec3 f_normal;
in vec2 f_uv;

uniform bool unlit;
uniform mat4 view;
uniform vec3 sun_dir;
uniform vec4 material_color;
uniform sampler2D diffuse_texture;

uniform sampler2DArray shadow_textures;

layout(std140, binding = 0) uniform LightMatricies {
    mat4 lights[CASCADE_COUNT];
};
uniform float cascade_distances[CASCADE_COUNT];

struct PointLight {
    vec4 position;
    vec3 color;
    float intensity;
};

#ifdef MAX_POINT_LIGHTS
layout(std140, binding = 1) uniform PointLights {
    PointLight point_lights[MAX_POINT_LIGHTS];
};
uniform int point_light_count;
#endif

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

#ifdef MAX_POINT_LIGHTS
vec3 calc_point_light(int index, vec3 camera_dir) {
    vec3 light_dir = normalize(point_lights[index].position.xyz - f_frag_pos);

    // Diffuse
    float diffuse_intensity = max(dot(f_normal, light_dir), 0.0);

    // Specular
    vec3 reflect_dir = reflect(-light_dir, f_normal);
    float specular_intensity = pow(max(dot(camera_dir, reflect_dir), 0.0), 64);

    // Attenuation
    float dst = length(point_lights[index].position.xyz - f_frag_pos);
    float attenuation = 1.0 / (1.0 + 0.14 * dst + 0.07 * (dst * dst));

    attenuation *= point_lights[index].intensity;

    vec3 ambient = vec3(AMBIENT_STRENGTH * attenuation);

#ifdef GRASS_SHADER
    vec3 diffuse = point_lights[index].color * attenuation * 0.2;
    vec3 specular = vec3(0.0);
#else
    vec3 diffuse = point_lights[index].color * attenuation * diffuse_intensity;
    vec3 specular = vec3(SPECULAR * attenuation * specular_intensity);
#endif


    return (ambient + diffuse + specular);
}
#endif

void main() {
    vec3 camera_position = -1.0 * vec3(view[3][0], view[3][1], view[3][2]);
    vec3 camera_dir = -1.0 * vec3(view[0][2], view[1][2], view[2][2]);
    vec3 camera_to_frag = normalize(camera_position - f_frag_pos);

    // Specular
    vec3 reflect_dir = reflect(-sun_dir, f_normal);
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

    float shadow;
    if (unlit) shadow = 0.0;
    else shadow = calc_shadow(layer);

    // Results
#ifdef GRASS_SHADER
    diffuse = mix(SUN_COLOR, diffuse, f_mesh_y + 0.09);
    vec3 result = (ambient + (1.0 - shadow) * diffuse) * material_color.rgb;
#else
    vec3 result;
    if (unlit) {
        result = material_color.rgb;
    }
    else {
        result = (ambient + (1.0 - shadow) * (diffuse + specular)) * material_color.rgb;
    }
#endif

#ifdef MAX_POINT_LIGHTS
    for (int i = 0; i < point_light_count; i++) {
        result += calc_point_light(i, camera_dir);
    }
#endif

    vec4 texture_sample = texture(diffuse_texture, f_uv);

#if 0
    if (layer == 0) texture_sample = vec4(1.0, 0.0, 0.0, 1.0);
    else if (layer == 1) texture_sample = vec4(0.0, 1.0, 0.0, 1.0);
    else if (layer == 2) texture_sample = vec4(0.0, 0.0, 1.0, 1.0);
#endif

    fragment_color = vec4(result, 1.0) * texture_sample;

#ifdef GRASS_SHADER
    if (fragment_color.a < 1.0) discard;
#endif

}

#endif
