internal void init_particles() {
    allocate_array(particles, PARTICLE_AMMOUNT);
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
                default :
                    log_print("Particle undefined!\n");
                    texture = "restt2.png";
                    break;
            }
            draw_particle(particle->position, particle->size, get_texture(texture), particle->color);
        }
    }
}

internal Particle spawn_fire_particle(Vector3 root_pos) {
    Particle particle;

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

    Vector3 target = make_vector3(root_pos.x, root_pos.y + 1.0f, root_pos.z);
    Vector3 velocity_vector = normalized(target - particle.position);

    particle.color    = make_color(rand_color, rand_color, rand_color, 128);
    particle.life     = 1.0f;
    particle.velocity = velocity_vector;
    particle.type     = 0;

    return particle;
}

internal Particle spawn_smoke_particle(Vector3 root_pos) {
    Particle particle;

    u8 rand_color = (rand() % 256);

    f32 rand_x = rand_f_range(-2.0f, 2.0f);
    f32 rand_z = rand_f_range(-2.0f, 2.0f);

    particle.position = make_vector3(root_pos.x + 0.20f, root_pos.y + 2.4f, root_pos.z + 0.16f);

    Vector3 target = make_vector3(root_pos.x + rand_x, root_pos.y + 10.0f, root_pos.z + rand_z);
    Vector3 velocity_vector = normalized(target - particle.position);

    particle.color        = make_color(rand_color, rand_color, rand_color, 64);
    particle.life         = rand_f_range(1.0f, 4.0f);
    particle.max_lifetime = particle.life;
    particle.velocity     = velocity_vector;
    particle.type         = 1;

    return particle;
}

internal void update_particle(f32 delta_time, Vector3 root_pos) {
    fire_spawnrate_timer += delta_time;
    smoke_spawnrate_timer += delta_time;

    // Add new particles
    if (smoke_spawnrate_timer >= SMOKE_SPAWNRATE) {
        for (u32 i = 0; i < 25; i++) {
            if (particles.is_full()) break;

            Particle particle = spawn_smoke_particle(root_pos);
            particles.add(particle);
        }
        smoke_spawnrate_timer -= SMOKE_SPAWNRATE;
    }

    if (fire_spawnrate_timer >= FIRE_SPAWNRATE) {
        for (u32 i = 0; i < 20; i++) {
            if (particles.is_full()) break;

            Particle particle = spawn_fire_particle(root_pos);
            particles.add(particle);
        }
        fire_spawnrate_timer -= FIRE_SPAWNRATE;
    }

    // Update existing particles
    for (i32 i = particles.length - 1; i >= 0; i--) {
        Particle &particle = particles[i];
        particle.life -= delta_time;

        if (particle.life <= 0.0f) {
            particles.fast_remove(i);
        }

        // Fire
        if (particle.type == 0) {
            f32 size = lerp(0.6, 0.3f, 1.0f - particle.life);
            particle.size = make_vector2(size, size);

            particle.position += particle.velocity * delta_time;
            particle.color.a  = u8(lerp(128.0f, 0.0f, 1.0f - particle.life));
        }
        // Smoke
        else if (particle.type == 1) {
#if 0
            {
                Vector3 start = make_vector3(root_pos.x + 0.16f, root_pos.y + 2.2f, root_pos.z + 0.17f);
                Vector3 random_offset = make_vector3(rand_f_range(-0.7f, 0.7f), 0.0f, rand_f_range(-0.7f, 0.7f));
                Vector3 next_point =
                    bezier_point(
                        start,
                        make_vector3(start.x, start.y + 1.0f, start.z),
                        make_vector3(start.x, start.y + 2.0f, start.z - 1.0f),
                        make_vector3(start.x, start.y + 2.5f, start.z - 2.5f),
                        1 - (particle.life / particle.max_lifetime)
                    );

                Vector3 velocity_vector = normalized((next_point + random_offset) - particle.position);
                particle.velocity = velocity_vector;
            }
#endif
            f32 size = lerp(0.3f, 0.8f, 1 - (particle.life / particle.max_lifetime));
            particle.size = make_vector2(size, size);

            particle.position += particle.velocity * delta_time;
            particle.color.a  = u8(lerp(64.0f, 0.0f, 1 - (particle.life / particle.max_lifetime)));
        }
    }
    draw_particles();
}