#if !defined(LIGHT_H)
#define LIGHT_H

#define DO_SHADOW_CASCADES

const u32 SHADOW_CASCADE_COUNT = 3;
const f32 SHADOW_SPLIT_LAMBDA = 0.7f;

const u32 SHADOW_SIZE = 2048;

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
    // u32 shadow_texture;

    Vector3 sun_direction;
    Matrix4x4 sun_view;
    Matrix4x4 sun_projection;
    Matrix4x4 sun_mvp;

#ifdef DO_SHADOW_CASCADES

    u32 lights_buffer_index;
    u32 lights_buffer_object;

    u32 shadow_cascades_texture;

    Matrix4x4 cascade_projections[SHADOW_CASCADE_COUNT];
    Matrix4x4 cascade_mvps[SHADOW_CASCADE_COUNT];
    f32 split_distances[SHADOW_CASCADE_COUNT + 1];
#endif
};

internal void init_light_data(LightData &data) {
    glGenFramebuffers(1, &data.frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, data.frame_buffer);

#ifdef DO_SHADOW_CASCADES
    {
        glGenTextures(1, &data.shadow_cascades_texture);
        glBindTexture(GL_TEXTURE_2D_ARRAY, data.shadow_cascades_texture);
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, SHADOW_SIZE, SHADOW_SIZE, SHADOW_CASCADE_COUNT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, V4_ONE.data);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, data.shadow_cascades_texture, 0);
    }
#endif

    glDrawBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    data.sun_projection = ortho(-50.0f, 50.0, -70.0f, 70.0f, 70.0f, -70.0f);
}

#endif // LIGHT_H
