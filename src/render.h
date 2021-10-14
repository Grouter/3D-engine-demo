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

#endif
