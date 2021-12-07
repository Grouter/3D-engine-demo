#ifdef VERTEX

in vec3 position;

uniform mat4 model;

void main() {
    gl_Position = model * vec4(position, 1.0);
}

#endif

#ifdef GEOMETRY

layout(triangles, invocations = CASCADE_COUNT) in;
layout(triangle_strip, max_vertices = 3) out;

layout(std140, binding = 0) uniform LightMatricies {
    mat4 lights[CASCADE_COUNT];
};

void main() {
    for (int i = 0; i < gl_in.length(); i++) {
        gl_Layer = gl_InvocationID;
        gl_Position = lights[gl_InvocationID] * gl_in[i].gl_Position;

        EmitVertex();
    }

    EndPrimitive();
}

#endif

#ifdef FRAGMENT

void main() {}

#endif