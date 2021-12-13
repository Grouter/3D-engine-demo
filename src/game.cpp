internal void init_game() {
    srand(42069);

    game_state.time_elapsed = 0.0f;
    game_state.bird_spawn_timer = 0.0f;

    game_state.camera = create_camera(VIRTUAL_WINDOW_W, VIRTUAL_WINDOW_H, 80.0f);
    game_state.camera.rotation.x = 25.0f;
    game_state.camera.rotation.y = 90.0f;

    game_state.camera.position.y = 10.0f;
    game_state.camera.position.z = 10.0f;

    game_state.ortho_proj = ortho(-1.0f, 1.0f, VIRTUAL_WINDOW_W_2D, VIRTUAL_WINDOW_H_2D);

    // Init and load resources
    init_resources(game_state.resources);
    load_material_file();
    load_mesh_file();
    allocate_entity_storage(game_state.entities);

    // Light
    init_light_data(game_state.light_data);
    init_light_buffers(game_state.light_data);
    game_state.light_data.sun_direction = normalized(make_vector3(-0.4f, -1.0f, -0.2f));

    // Load world
    load_world_file(game_state.entities);

    // Spawn rocks
    {
        f32 a_part = TWO_PI / ROCKS;
        f32 current_a = 0.0f;

        for (i32 i = 0; i < ROCKS; i++) {
            Entity *root = create_rock_formation(game_state.entities);

            f32 radius = rand_f_range(100, 200);

            root->position.x = sinf(current_a) * radius;
            root->position.y = rand_f_range(-50, 150);
            root->position.z = cosf(current_a) * radius;
            root->scale = make_vector3(8.0f);

            current_a += a_part;
        }
    }

    // Plant grass
    {
        stbi_set_flip_vertically_on_load(true);

        i32 texture_width, texture_height, nr_channels;
        u8 *pixels = stbi_load("textures/grass_plant_mask.png", &texture_width, &texture_height, &nr_channels, 0);

        // How big of an area (around the world center) should be mapped to the spawn mask texture
        Vector2 spawn_area = make_vector2(115.0f, 115.0f);

        f32 spawn_increment = 2.0f;

        Vector2 spawn_offset = make_vector2(-2.0f, -1.0f);
        Vector2 spawn = {};

        for (spawn.x = 0.0f ;spawn.x < spawn_area.x; spawn.x += spawn_increment) {
            for (spawn.y = 0.0f; spawn.y < spawn_area.y; spawn.y += spawn_increment) {
                // Convert to classic UVs
                Vector2 unit_coordinates;
                unit_coordinates.x = spawn.x / spawn_area.x;
                unit_coordinates.y = spawn.y / spawn_area.y;

                // Clamp to range <0, 1>
                unit_coordinates.x = 1.0f - max(0.0f, min(1.0f, unit_coordinates.x));
                unit_coordinates.y = max(0.0f, min(1.0f, unit_coordinates.y));

                i32 texture_x = (i32)(unit_coordinates.x * texture_width);
                i32 texture_y = (i32)(unit_coordinates.y * texture_height);

                i32 pixel_index = texture_y * texture_width + texture_x;

                // *4 because RGBA values are stored
                u8 value = pixels[pixel_index * 4];

                if (value != 0) {
                    Matrix4x4 grass = identity();
                    translate(
                        grass,
                        (spawn.x - spawn_area.x * 0.5f) + spawn_offset.x,
                        -0.2f,
                        (spawn.y - spawn_area.y * 0.5f) + spawn_offset.y
                    );
                    rotate(grass, 0.0f, rand_f_range(0.0f, TWO_PI), 0.0f);

                    game_state.entities.grass_data.add(grass);

                    if (game_state.entities.grass_data.is_full()) break;
                }
            }

            if (game_state.entities.grass_data.is_full()) break;
        }

        log_print("Spawned: %d grass patches\n", game_state.entities.grass_data.length);
        stbi_image_free(pixels);
    }
}

internal void tick(f32 dt) {
    camera_handle_input(game_state.camera);

    camera_animate(game_state.camera, game_state.resources.camera_animation, dt);
    camera_update(game_state.camera);

    game_state.bird_spawn_timer += dt;
    if (game_state.bird_spawn_timer >= BIRD_SPAWN_TIME) {
        Entity *bird = create_bird(game_state.entities);
        BirdData *bird_data = &game_state.entities.entity_data[bird->data].bird_data;

        {
            Vector2 r_u = rand_unit_v2() * 200.0f;
            bird->position.x = r_u.x;
            bird->position.y = BIRD_HEIGHTS + rand_f_range(0.0f, 5.0f);
            bird->position.z = r_u.y;
        }

        {
            Vector3 rand_position;
            rand_position.x = (f32)(rand() % 50) - 25.0f;
            rand_position.y = BIRD_HEIGHTS;
            rand_position.z = (f32)(rand() % 50) - 25.0f;

            Vector3 dir = rand_position - bird->position;
            normalize(dir);

            bird_data->direction = make_vector2(dir.x, dir.z);

            f32 angle = atan2(dir.x, dir.z) + PI;
            bird->rotation.y = angle;
        }

        game_state.bird_spawn_timer -= BIRD_SPAWN_TIME;
    }

    // Update entities
    {
        Entity *it;

        bucket_array_foreach(game_state.entities.base_entities, it) {
            if (it->type == EntityType_BIRD) {
                BirdData *bird_data = &game_state.entities.entity_data[it->data].bird_data;

                bird_data->life += dt;

                if (bird_data->life >= BIRD_LIFESPAN) {
                    it->flags.destroy = 1;
                    continue;
                }

                // Move towads target
                {
                    // @Todo: rotate to direction

                    it->position.x += (bird_data->direction.x * BIRD_SPEED * dt);
                    it->position.z += (bird_data->direction.y * BIRD_SPEED * dt);
                }
            }
            else if (it->type == EntityType_FLYING_ROCK) {
                FlyingRockData *rock_data = &game_state.entities.entity_data[it->data].flying_rock_data;

                it->rotation.y += dt * 0.01f * rock_data->rotation_direction;

                it->position.y += sinf(game_state.time_elapsed * 0.2f) * 0.001f * rock_data->rotation_direction;
            }
            else if (it->type == EntityType_SHIP) {
                ShipData *data = &game_state.entities.entity_data[it->data].ship_data;

                if (data->original_y_position == FLT_MAX) {
                    data->original_y_position = it->position.y;
                }

                it->position.y = data->original_y_position + sinf(game_state.time_elapsed * 0.1f) * 0.8f;
                it->rotation.x = sinf(game_state.time_elapsed * 0.2f) * 0.02f;
                it->rotation.z = sinf(game_state.time_elapsed * 0.2f) * 0.05f;
            }
        }}
    }

    // Calculate transforms
    {
        FlyingRockTransformHierarchy &hierarchy = game_state.entities.flying_rock_transforms;

        hierarchy.local.clear();
        hierarchy.lookups.clear();
        hierarchy.results.clear();

        u32 last_root = 0;
        u32 last_sub = 0;

        Entity *it;
        bucket_array_foreach(game_state.entities.base_entities, it) {
            if (it->type == EntityType_FLYING_ROCK) {
                FlyingRockData *data = &game_state.entities.entity_data[it->data].flying_rock_data;

                Matrix4x4 local = to_transform(it->position, it->rotation, it->scale);
                hierarchy.local.add(local);

                if (data->hierarchy_level == 0) {
                    last_root = (u32)hierarchy.local.length - 1;

                    hierarchy.results.add(local);
                    it->transform = local;
                }
                else if (data->hierarchy_level == 1) {
                    last_sub = (u32)hierarchy.local.length - 1;

                    Matrix4x4 result = multiply(hierarchy.results[last_root], local);
                    hierarchy.results.add(result);
                    it->transform = result;
                }
                else {
                    Matrix4x4 result = multiply(hierarchy.results[last_sub], local);
                    hierarchy.results.add(result);
                    it->transform = result;
                }
            }
            else if (it->type == EntityType_LAMP) {
                it->transform = to_transform(it->position, it->rotation, it->scale);

                LampData *data = &game_state.entities.entity_data[it->data].lamp_data;

                if (game_state.light_data.point_lights.length <= data->point_light_index) continue;

                // Side because lamp mesh is rotated wrong!
                Vector3 light_pos = it->position + (get_side_vector(it->transform) * -0.8f);
                light_pos.y += 1.5f;
                game_state.light_data.point_lights[data->point_light_index].position = light_pos;
            }
            else {
                it->transform = to_transform(it->position, it->rotation, it->scale);
            }
        }}
    }

    // Handle particles
    {
        update_particle(dt, ROOT_POSITION, NEW_PARTICLES);
        draw_particles();
    }

    remove_flagged_entities(game_state.entities);
}

internal void render() {
    // Do CSM!
#ifdef DO_SHADOW_CASCADES
    {
        calc_shadowmap_split_distances(game_state.camera, game_state.light_data);
        calc_cascade_matricies(game_state.camera, game_state.light_data);
    }
#else
    {
        // Calculate sun transform for shadow calulations
        {
            Vector3 forward = normalized(game_state.light_data.sun_direction);
            Vector3 side    = normalized(cross(V3_UP, forward));
            Vector3 up      = normalized(cross(forward, side));

            game_state.light_data.sun_view = {
                side.x, up.x, forward.x, 0.0f,
                side.y, up.y, forward.y, 0.0f,
                side.z, up.z, forward.z, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f,
            };
    }

        game_state.light_data.sun_mvp = multiply(game_state.light_data.sun_projection, game_state.light_data.sun_view);

        for (int i = 0; i < SHADOW_CASCADE_COUNT; i++) {
            game_state.light_data.cascade_mvps[i] = game_state.light_data.sun_mvp;
        }
    }
#endif

    // Upload sun projections
    {
        glBindBuffer(GL_UNIFORM_BUFFER, game_state.light_data.shadow_uniform_buffer);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrix4x4) * SHADOW_CASCADE_COUNT, game_state.light_data.cascade_mvps);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    // Upload point light data
    {
        glBindBuffer(GL_UNIFORM_BUFFER, game_state.light_data.point_lights_uniform_buffer);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PointLight) * MAX_POINT_LIGHTS, game_state.light_data.point_lights.data);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    // Render all entities
    {
        Entity *it;
        bucket_array_foreach(game_state.entities.base_entities, it) {
            render_entity(*it, it->transform);
        }}

        Matrix4x4 *grass;
        array_foreach(game_state.entities.grass_data, grass) {
            _draw_calls_grass.data.add(*grass);
        }
    }

    // Draw shadow maps
    {
        glEnable(GL_DEPTH_TEST);

        glBindFramebuffer(GL_FRAMEBUFFER, game_state.light_data.frame_buffer);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_TEXTURE_2D_ARRAY, game_state.light_data.shadow_maps, 0);
        glViewport(0, 0, SHADOW_SIZE, SHADOW_SIZE);
        glClear(GL_DEPTH_BUFFER_BIT);

        set_shader(ShaderResource_Shadow);

        flush_draw_calls_shadow();
    }

    // HDR pass
    {
        glBindFramebuffer(GL_FRAMEBUFFER, game_state.hdr_framebuffer);
        glViewport(0, 0, HDR_TARGET_W, HDR_TARGET_H);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw 3Ds
        {
            flush_draw_calls();
            flush_draw_calls_grass();
        }

        // Skybox
        {
            glDepthMask(GL_FALSE);
            set_shader(ShaderResource_Skybox);
            Mesh *cube = &game_state.resources.meshes[MeshResource_CubeMap];
            glBindVertexArray(cube->vao);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, game_state.skybox_cubemap);

            i32 loc = glGetUniformLocation(current_shader->handle, "skybox");
            if (loc >= 0) {
                glUniform1i(loc, 0);
            }
            else {
                log_print("Shader set skybox loc error!\n");
            }

            Matrix4x4 view = game_state.camera.transform;
            view.table[3][0] = 0.0f;
            view.table[3][1] = 0.0f;
            view.table[3][2] = 0.0f;
            set_shader_matrix4x4("view", view);
            set_shader_matrix4x4("projection", game_state.camera.perspective);

            glDrawElements(GL_TRIANGLES, (i32)cube->indicies.length, GL_UNSIGNED_INT, 0);
            glDepthMask(GL_TRUE);
        }

        // Draw particles
        {
            glDepthMask(GL_FALSE);
            set_shader(ShaderResource_Particles);
            set_shader_matrix4x4("view", game_state.camera.transform);
            set_shader_matrix4x4("projection", game_state.camera.perspective);

            flush_draw_calls_particles();
            glDepthMask(GL_TRUE);
        }
    }

    // Final pass
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(
            game_state.viewport.left,
            game_state.viewport.bottom,
            game_state.viewport.width,
            game_state.viewport.height
        );

        set_shader(ShaderResource_HDR);
        set_shader_sampler("hdr_buffer", 0, game_state.post_color_buffer);

        Mesh *hdr_quad = &game_state.resources.meshes[MeshResource_HDR_Quad];
        glBindVertexArray(hdr_quad->vao);

        glDrawElements(GL_TRIANGLES, (i32)hdr_quad->indicies.length, GL_UNSIGNED_INT, 0);

        // Draw 2Ds
        {
            glDisable(GL_DEPTH_TEST);
            set_shader(ShaderResource_2D);
            set_shader_matrix4x4("projection", game_state.ortho_proj);

            set_shader_int("diffuse_alpha_mask", 0);
            flush_2d_shapes_draw_calls();

            set_shader_int("diffuse_alpha_mask", 1);
            flush_font_draw_calls();
            glEnable(GL_DEPTH_TEST);
        }
    }
}
