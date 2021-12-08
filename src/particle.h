#if !defined(PARTICLE_H)
#define PARTICLE_H

#define PARTICLE_AMMOUNT 500

struct Particle {
    Vector3 position;
    Vector3 velocity;

    Color color;
    f32 life; 
};

global Array<Particle> particles;
global u32 last_used = 0;

#endif