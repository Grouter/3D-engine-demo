#if !defined(RENDER_H)
#define RENDER_H

#define MAX_DRAW_CALLS 1024
#define MAX_PARTICLE_DRAW_CALLS 1024

union DrawCallFlags {
    u64 raw;

    struct {
        u8 shader;
        u32 material;
    };
};

struct DrawCallData {
    DrawCallFlags flags;
    SubMeshInfo   info;
    Mesh          *mesh;
    Matrix4x4     transform;
};

struct DrawCallData2D {
    Vector4   color     = V4_ONE;
    Vector2   uv_offset = {};
    Vector2   uv_scale  = V2_ONE;
    Matrix4x4 transform;
};

struct DrawCallDataParticle {
    u32 texture;
    Vector2   uv_offset = {};
    Vector2   uv_scale  = V2_ONE;
    Matrix4x4 transform;
};

struct DrawCallBuffer2D {
    u32 vao;
    u32 instance_buffer;
    Array<DrawCallData2D> data;
};

struct DrawCallBufferParticles {
    u32 vao;
    u32 instance_buffer;
    Array<DrawCallDataParticle> data;
};

global DrawCallBuffer2D _font_draw_calls[FontResource_COUNT];
global DrawCallBuffer2D _2d_shapes_draw_calls;
global DrawCallBufferParticles _particle_draw_calls;
global Array<DrawCallData> _draw_calls;

#endif
