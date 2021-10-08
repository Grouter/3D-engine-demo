#ifdef VERTEX

in vec3 position;
in vec3 normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model; // @Temporary

out vec3 f_normal;

void main() {
    // @Speed: slow!!!
    f_normal = mat3(transpose(inverse(model))) * normal;

    gl_Position = projection * view * model * vec4(position, 1.0);
}

#endif

#ifdef FRAGMENT

const vec3 LIGHT_DIR = vec3(0.0, 0.0, 0.5);
const vec3 LIGHT_COLOR = vec3(1.0, 1.0, 1.0);

const float AMBIENT_STRENGTH = 0.2;

const vec3 OBJ_COLOR = vec3(0.0, 1.0, 0.0);

in vec3 f_normal;

out vec4 fragment_color;

void main() {
    vec3 ambient = AMBIENT_STRENGTH * LIGHT_COLOR;

    float diffuse_s = max(dot(f_normal, LIGHT_DIR), 0.0);
    vec3 diffuse = diffuse_s * LIGHT_COLOR;

    vec3 result = (ambient + diffuse) * OBJ_COLOR;

    fragment_color = vec4(result, 1.0);
}

#endif