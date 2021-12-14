#if !defined(PARTICLE_H)
#define PARTICLE_H

#define PARTICLE_AMMOUNT 2048
#define ROOT_POSITION {-9.5f, 0.25f, -16.77f}
#define SPAWN_OFFSET 0.35f
#define FIRE_SPAWNRATE 0.05f
#define SMOKE_SPAWNRATE 0.02f

struct Particle {
    Vector3 position;
    Vector3 velocity;
    Vector2 size;

    Color color;
    f32 life;
    f32 max_lifetime;
    u8 type;
};

global Array<Particle> particles;
global f32 fire_spawnrate_timer  = 0;
global f32 smoke_spawnrate_timer = 0;


#endif