#if !defined(LIGHT_H)
#define LIGHT_H

const u32 SHADOW_CASCADE_COUNT = 3;
const f32 SHADOW_SPLIT_LAMBDA = 0.5f;

const u32 SHADOW_SIZE = 4096;

struct BBox {
    Vector3 near_top_left;
    Vector3 near_top_right;
    Vector3 near_bottom_left;
    Vector3 near_bottom_right;
    Vector3 far_top_left;
    Vector3 far_top_right;
    Vector3 far_bottom_left;
    Vector3 far_bottom_right;
};

struct LightData {
    u32 frame_buffer;
    u32 shadow_texture;

    Vector3 sun_direction;
    Matrix4x4 sun_view;
    Matrix4x4 sun_projection;
    Matrix4x4 sun_mvp;

#if 0
    u32 shadow_cascades_texture;
    Matrix4x4 cascade_projections[SHADOW_CASCADE_COUNT];
    f32 split_distances[SHADOW_CASCADE_COUNT + 1];
#endif
};

internal void init_light_data(LightData &data) {
    glGenFramebuffers(1, &data.frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, data.frame_buffer);

    {
        glGenTextures(1, &data.shadow_texture);
        glBindTexture(GL_TEXTURE_2D, data.shadow_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, SHADOW_SIZE, SHADOW_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, data.shadow_texture, 0);
    }

#if 0
    {
        glGenTextures(1, &data.shadow_cascades_texture);
        glBindTexture(GL_TEXTURE_2D_ARRAY, data.shadow_cascades_texture);
        glTextureStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_DEPTH_COMPONENT32F, 1024, 1024, SHADOW_CASCADE_COUNT);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, data.shadow_texture, 0);
    }
#endif

    glDrawBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    data.sun_projection = ortho(-50.0f, 50.0, -70.0f, 70.0f, 70.0f, -70.0f);
}

#endif // LIGHT_H
