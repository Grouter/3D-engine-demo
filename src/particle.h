#if !defined(PARTICLE_H)
#define PARTICLE_H

#define PARTICLE_AMMOUNT 2048
#define ROOT_POSITION {-9.5f, 0.25f, -16.75f}
#define NEW_PARTICLES 4
#define SPAWN_OFFSET 0.4f

struct Particle {
    Vector3 position;
    Vector3 velocity;
    Vector2 size;

    Color color;
    f32 life;
    byte type;
};

global Array<Particle> particles;
global u32 last_used = 0;

#endif