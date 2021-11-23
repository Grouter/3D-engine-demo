#if !defined(RESOURCES_H)
#define RESOURCES_H

enum ShaderResource {
    ShaderResource_Default,
    ShaderResource_2D
};

const u64 MeshResource_Quad = 0;

enum FontResource {
    FontResource_Small,
    FontResource_Medium,
    FontResource_Big,

    FontResource_COUNT
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
    Array<Font>     fonts;

    CameraAnimation camera_animation;
};

#endif
