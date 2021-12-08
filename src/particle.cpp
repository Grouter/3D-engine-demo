internal void init_particles() {

    allocate_array(particles, PARTICLE_AMMOUNT);

    for (u32 i = 0; i < PARTICLE_AMMOUNT; i++) {
        particles.add(Particle());
    }
        
}

internal void draw_particles() {
    
    Particle *particle;
    array_foreach(particles, particle) {

        if (particle->life > 0.0f) {
            draw_particle(particle->position, {0.1f, 0.1f}, get_texture("trans.png"));
        }
    }   
}

internal u32 first_free_particle() {    

    for (u32 i = last_used; i < PARTICLE_AMMOUNT; i++) {
        if (particles[i].life <= 0.0f) {
            last_used = i;
            return i;
        }
    }

    for (u32 i = 0; i < last_used; i++) {
        if (particles[i].life <= 0.0f) {
            last_used = i;
            return i;
        }
    }
    
    last_used = 0;
    return 0;
}

internal void respawn_particle(Particle &particle, Vector3 root_pos) {
    f32 rand_x = rand_f_range(-0.5f, 0.5f);
    f32 rand_y = rand_f_range(-0.5f, 0.5f);
    f32 rand_z = rand_f_range(-0.5f, 0.5f);

    u8 rand_color = (rand() % 256);

    f32 rand_velo = rand_f_range(-1.0f, 1.0f);

    //particle.position = root_pos + random;
    particle.position.x = root_pos.x + rand_x;
    particle.position.y = root_pos.y + rand_y;
    particle.position.z = root_pos.z + rand_z;

    particle.color    = make_color(rand_color, rand_color, rand_color, 255);
    particle.life     = 1.0f;
    particle.velocity = {rand_velo, -1.0f, rand_velo};
}

internal void update_particle(f32 delta_time, Vector3 root_pos, u32 new_particles) {

    for (u32 i = 0; i < new_particles; i++) {
        u32 free_particle = first_free_particle();
        respawn_particle(particles[free_particle], root_pos);
    }

    for (u32 i = 0; i < PARTICLE_AMMOUNT; i++) {
        Particle &p = particles[i];
        p.life -= delta_time;
        
        if (p.life > 0.0f) {
            p.position -= p.velocity * delta_time;
            p.color.a  -= delta_time * 2.5f;
        }
    }
}