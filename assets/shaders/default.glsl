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

const vec3  SUN_DIR          = vec3(0.0, 0.0, 1.0);
const vec3  SUN_COLOR        = vec3(1.0, 0.945, 0.788);
const float AMBIENT_STRENGTH = 0.2;

const float SPECULAR = 0.5;

in vec3 f_frag_pos;
in vec3 f_normal;
in vec2 f_uv;

uniform vec3 camera_position;
uniform vec4 material_color;
uniform sampler2D diffuse_texture;

out vec4 fragment_color;

void main() {
    vec3 view_dir = normalize(camera_position - f_frag_pos);
    vec3 reflect_dir = reflect(-SUN_DIR, f_normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
    vec3 specular = SPECULAR * spec * SUN_COLOR;

    vec3 ambient = AMBIENT_STRENGTH * SUN_COLOR;
    vec3 sun_dir_n = normalize(SUN_DIR);

    float diffuse_s = max(dot(f_normal, SUN_DIR), 0.0);
    vec3 diffuse = diffuse_s * SUN_COLOR;

    vec3 result = (ambient + diffuse + specular) * material_color.rgb;

    vec4 texture_sample = texture(diffuse_texture, f_uv);

    fragment_color = vec4(result, 1.0) * texture_sample;
}

#endif