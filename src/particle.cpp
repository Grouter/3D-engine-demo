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

            char *texture = {};
            switch (particle->type) {

                case 0:
                    texture = "fire_particle.png";
                    break;
                case 1:
                    texture = "smoke_particle.png";
                    break;
            }
            draw_particle(particle->position, particle->size, get_texture(texture), particle->color);
        }
    }
}

internal u32 first_free_particle() {

    for (u32 i = last_used; i < PARTICLE_AMMOUNT; i++) {
        if (particles[i].life <= 0.0f) {
            particles[i].type = particles[i].type ? 0 : 1;
            last_used = i;
            return i;
        }
    }

    for (u32 i = 0; i < last_used; i++) {
        if (particles[i].life <= 0.0f) {
            particles[i].type = particles[i].type ? 0 : 1;
            last_used = i;
            return i;
        }
    }

    last_used = 0;
    particles[0].type = particles[0].type ? 0 : 1;
    return 0;
}

internal void respawn_particle(Particle &particle, Vector3 root_pos) {

    f32 rand_x = rand_f_range(-SPAWN_OFFSET, SPAWN_OFFSET);
    f32 rand_z = rand_f_range(-SPAWN_OFFSET, SPAWN_OFFSET);

    u8 rand_color = (rand() % 256);

    // Spwaning only in front-facing triangle
    {
        if (rand_x < 0 && rand_z < 0) {
            rand_x *= -1;
        }
        if (rand_x < 0 && rand_z <= SPAWN_OFFSET - (SPAWN_OFFSET - abs(rand_x))) {
            rand_x *= -1;
        }
        if (rand_z < 0 && rand_x <= SPAWN_OFFSET - (SPAWN_OFFSET - abs(rand_z))) {
            rand_z *= -1;
        }
    }
    
    particle.position.x = root_pos.x + rand_x;
    particle.position.y = root_pos.y;
    particle.position.z = root_pos.z + rand_z;

    Vector3 target = {root_pos.x, root_pos.y + 1.0f, root_pos.z};
    Vector3 velocity_vector = target - particle.position;
    normalize(velocity_vector);

    particle.color    = make_color(rand_color, rand_color, rand_color, 255);
    particle.life     = 1.0f;
    particle.size     = {0.3f, 0.3f};
    particle.velocity = velocity_vector;

}

internal void update_particle(f32 delta_time, Vector3 root_pos, u32 new_particles) {
    for (u32 i = 0; i < new_particles; i++) {
        u32 free_particle = first_free_particle();
        respawn_particle(particles[free_particle], root_pos);
    }

    Particle *particle;
    array_foreach(particles, particle) {
        particle->life -= delta_time;

        f32 rand_chance = rand_f_range(0.0f, 100.0f);

        if (particle->life > 0.0f) {
            particle->position += particle->velocity * delta_time;

            // Fade doesnt work when casted to u8
            particle->color.a -= particle->life * 0.7;
        }

        // Fire particle changes to smoke particle
        else if (rand_chance > 50.0f && particle->life <= 0.0f && particle->type == 0) {
            particle->life        = 2.0f;
            particle->type        = 1;
            particle->size        = {0.3f, 0.3f};
            particle->position    = {root_pos.x + 0.16f, root_pos.y + 2.2f, root_pos.z + 0.17f};
        }
    }
}