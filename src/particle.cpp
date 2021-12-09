internal void init_particles() {
    allocate_array(particles, PARTICLE_AMMOUNT);

    // @Note(M): Don't know why is it neccessary to have all the field initialized here...
    // Why didn't we add and remove the particles as we need?
    for (u32 i = 0; i < PARTICLE_AMMOUNT; i++) {
        particles.add(Particle {});
    }
}

internal void draw_particles() {
    Particle *particle;
    array_foreach(particles, particle) {
        if (particle->life > 0.0f) {
            char *texture = {};

            // @Note(M):
            // This is fine, but make sure to handle the null case!!
            // We can assign the white texture for example if the type is invalid and print some errors
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
    // @Note(M):
    // I think this work is really uneccessary if you used the basic array funtions like .add and .fast_remove.
    // Also you can use .allocate instead of .add, and array will directly return the allocate memory pointer
    // so you don't have to worry about c++ compiler not optimizing out the memcopies.
    //
    // Im glad to see, that you tried to optimize and add to the most recently used spot to try and use the cached memory pages,
    // but you are introducing something else like internal fragmentation, which can be (most likely) in the end much worse.
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

    // @Note(M): I still don't understand this.
    // The root should be the root and let the particles spawn around...
    // When a system introduces some hack, it is usually a symptom of something not being correct (or a shitty API).

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

    Vector3 target = make_vector3(root_pos.x, root_pos.y + 1.0f, root_pos.z);
    Vector3 velocity_vector = normalized(target - particle.position);

    particle.color    = make_color(rand_color, rand_color, rand_color, 255);
    particle.life     = 1.0f;
    particle.size     = make_vector2(0.3f, 0.3f);
    particle.velocity = velocity_vector;

}

// @Note(M):
// The data transformations should be more like (iterating from the back):
// update particle lifetime
// if particle is dead, remove (.fast_remove)
// if the particle is smoke, do this
// if the particle is fire, do this
// if particle is x do y...
// draw (the rendering can be in the same function call as this update) - cache coherency
//
// In my opinion, that would be much more readable and more simple.
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

            // @Todo @Robustness : this should rather be lerp from alpha start to alpha end based on t=particle_current_life/particle_max_life
            // (we have a lerp() function)
            // That will allow for a more robust code and for longer/shorter particle lifetimes.
            // Also we will have exact and correct mathematic control over the alpha values because life * 0.7 can be anything really...
            // It would be very nice to have bezier interpolations, but that is not a priority on this project right now.
            particle->color.a = particle->life * 0.7f;
        }
        // Fire particle changes to smoke particle
        else if (rand_chance > 50.0f && particle->life <= 0.0f && particle->type == 0) {
            particle->life        = 2.0f;
            particle->type        = 1;
            particle->size        = make_vector2(0.3f, 0.3f);
            particle->position    = make_vector3(root_pos.x + 0.16f, root_pos.y + 2.2f, root_pos.z + 0.17f);
        }
    }
}