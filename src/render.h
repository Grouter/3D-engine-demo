#if !defined(RENDER_H)
#define RENDER_H

#define MAX_DRAW_CALLS 1024

// How many bits per property. These are not bit masks!
// Sum of these bits must add up to the DrawCallData's flag property size.
enum RenderDataFlagBits : u8 {
    ShaderBits   = 1,
    MaterialBits = 16,
};

struct DrawCallData {
    u64         flags;
    SubMeshInfo info;
    Mesh        *mesh;
    Matrix4x4   transform;
};

struct DrawCallData2D {
    Vector4   color     = V4_ONE;
    Vector2   uv_offset = {};
    Vector2   uv_scale  = V2_ONE;
    Matrix4x4 transform;
};

struct DrawCallBuffer2D {
    u32 vao;
    u32 instance_buffer;
    Array<DrawCallData2D> data;
};

global DrawCallBuffer2D _font_draw_calls[FontResource_COUNT];
global DrawCallBuffer2D _2d_shapes_draw_calls;
global Array<DrawCallData> _draw_calls;

#endif
