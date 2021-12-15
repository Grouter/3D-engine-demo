#if !defined(PARTICLE_H)
#define PARTICLE_H

#define PARTICLE_AMMOUNT 2048
#define ROOT_POSITION {-9.5f, 0.25f, -16.77f}
#define FLAME_ROOT_POSITION {9.0f, 4.965f, -34.99f}
#define SPAWN_OFFSET 0.35f
#define FIRE_SPAWNRATE 0.04f
#define SMOKE_SPAWNRATE 0.03f
#define FLAME_SPAWNRATE 2.0f

struct Particle {
    Vector3 position;
    Vector3 velocity;
    Vector2 size;

    Color color;
    f32 life;
    f32 max_lifetime;
    u8 type;
};

Array<Particle> particles;
f32 fire_spawnrate_timer  = 0;
f32 smoke_spawnrate_timer = 0;
f32 flame_spawnrate_timer = 0;
const Vector3 WIND = make_vector3(-0.4f, 0.0f, 0.2f);


#endif