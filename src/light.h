#if !defined(LIGHT_H)
#define LIGHT_H

const u32 SHADOW_SIZE = 8192;

struct LightData {
    u32 frame_buffer;
    u32 shadow_texture;

    Vector3 sun_direction;
    Matrix4x4 sun_projection;
    Matrix4x4 sun_mvp;
};

internal void init_light_data(LightData &data) {
    glGenFramebuffers(1, &data.frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, data.frame_buffer);

    glGenTextures(1, &data.shadow_texture);
    glBindTexture(GL_TEXTURE_2D, data.shadow_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, SHADOW_SIZE, SHADOW_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, data.shadow_texture, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    data.sun_projection = ortho(-100.0f, 100.0, -70.0f, 70.0f, 70.0f, -70.0f);
}

#endif // LIGHT_H
