#if !defined(RESOURCES_H)
#define RESOURCES_H

enum ShaderResource {
    Default
};

struct Resources {
    ResourceCatalog shader_catalog;
    ResourceCatalog mesh_catalog;
    ResourceCatalog texture_catalog;
    ResourceCatalog material_catalog;

    Program         programs[1];
    Array<Mesh>     meshes;
    Array<u32>      textures;
    Array<Material> materials;

    CameraAnimation camera_animation;
};

#endif
