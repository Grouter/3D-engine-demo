#if !defined(RESOURCES_H)
#define RESOURCES_H

enum ShaderResource {
    Default
};

struct Resources {
    ResourceCatalog mesh_catalog;
    ResourceCatalog texture_catalog;
    ResourceCatalog material_catalog;

    Mesh     meshes[2];
    Program  programs[1];
    Array<u32>      textures;
    Array<Material> materials;
};

#endif
