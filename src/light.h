#if !defined(LIGHT_H)
#define LIGHT_H

// #define DO_SHADOW_CASCADES

#ifdef DO_SHADOW_CASCADES
const u32 SHADOW_CASCADE_COUNT = 3;
const u32 SHADOW_SIZE = 2048;
#else
const u32 SHADOW_CASCADE_COUNT = 1;
const u32 SHADOW_SIZE = 8192;
#endif

const f32 SHADOW_SPLIT_LAMBDA = 0.8f;

struct LightData {
    u32 frame_buffer;

    Vector3 sun_direction;

    Matrix4x4 sun_view;
    Matrix4x4 sun_projection;
    Matrix4x4 sun_mvp;

    u32 shadow_uniform_buffer;
    u32 shadow_maps;

    Matrix4x4 cascade_projections[SHADOW_CASCADE_COUNT];
    Matrix4x4 cascade_mvps[SHADOW_CASCADE_COUNT];
    f32 cascade_splits[SHADOW_CASCADE_COUNT + 1];
};

internal void init_light_data(LightData &data) {
    glGenFramebuffers(1, &data.frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, data.frame_buffer);

    glGenTextures(1, &data.shadow_maps);
    glBindTexture(GL_TEXTURE_2D_ARRAY, data.shadow_maps);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, SHADOW_SIZE, SHADOW_SIZE, SHADOW_CASCADE_COUNT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, V4_ONE.data);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, data.shadow_maps, 0);

    glDrawBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    data.sun_projection = ortho(-100.0f, 20.0, -100.0f, 100.0f, 100.0f, -100.0f);
}

#endif // LIGHT_H
