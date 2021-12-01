#ifdef VERTEX

in vec3 position;

uniform mat4 model;
uniform mat4 light;

void main() {
    gl_Position = light * model * vec4(position, 1.0);
}

#endif

#ifdef FRAGMENT

out float fragment_depth;

void main() {
    fragment_depth = gl_FragCoord.z;
}

#endif