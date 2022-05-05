#pragma once

#include "./mesh.h"
#include "./grid.h"
#include "./box.h"
#include "./camera.h"
#include "./material.h"
#include "./texture.h"
#include "../core/ray.h"
#include "../core/transform.h"
#include "../serialization/mesh.h"
#include "../serialization/texture.h"

struct SceneCounts {
    u32 cameras{1};
    u32 geometries{0};
    u32 grids{0};
    u32 boxes{0};
    u32 curves{0};
    u32 meshes{0};
    u32 materials{0};
    u32 textures{0};
    u32 lights{0};
};

struct Scene {
    SceneCounts counts;
    String file_path;

    AmbientLight ambient_light;

    Geometry *geometries{nullptr};
    Curve *curves{nullptr};
    Grid *grids{nullptr};
    Box *boxes{nullptr};
    Camera *cameras{nullptr};
    Mesh *meshes{nullptr};
    Texture *textures{nullptr};
    Material *materials{nullptr};
    Light *lights{nullptr};

    u64 last_io_ticks{0};
    bool last_io_is_save{false};

    Scene(SceneCounts counts,
          char *file_path = nullptr,
          Camera *cameras = nullptr,
          Geometry *geometries = nullptr,
          Grid *grids = nullptr,
          Box *boxes = nullptr,
          Curve *curves = nullptr,
          Material *materials = nullptr,
          Light *lights = nullptr,
          Mesh *meshes = nullptr,
          String *mesh_files = nullptr,
          Texture *textures = nullptr,
          String *texture_files = nullptr,
          memory::MonotonicAllocator *memory_allocator = nullptr
    ) : counts{counts},
        file_path{file_path},
        cameras{cameras},
        geometries{geometries},
        grids{grids},
        boxes{boxes},
        curves{curves},
        meshes{meshes},
        textures{textures},
        materials{materials},
        lights{lights}
    {
        memory::MonotonicAllocator temp_allocator;
        if (!memory_allocator && (meshes || textures)) {
            u32 capacity = 0;
            if (meshes)
                capacity += getTotalMemoryForMeshes(mesh_files, counts.meshes);
            if (textures)
                capacity += getTotalMemoryForTextures(texture_files, counts.textures);;
            temp_allocator = memory::MonotonicAllocator{capacity};
            memory_allocator = &temp_allocator;
        }
        if (meshes) {
            meshes = new(meshes) Mesh[counts.meshes];
            for (u32 i = 0; i < counts.meshes; i++) load(meshes[i], mesh_files[i].char_ptr, memory_allocator);
        }
        if (textures) {
            textures = new(textures) Texture[counts.textures];
            for (u32 i = 0; i < counts.textures; i++) load(textures[i], texture_files[i].char_ptr, memory_allocator);
        }
    }

    INLINE bool castRay(Ray &ray) const {
        static Ray local_ray;
        static Transform xform;

        bool found{false};
        bool current_found{false};

        Geometry *geo = geometries;

        for (u32 i = 0; i < counts.geometries; i++, geo++) {
            xform = geo->transform;
            if (geo->type == GeometryType_Mesh)
                xform.scale *= meshes[geo->id].aabb.max;

            xform.internPosAndDir(ray.origin, ray.direction, local_ray.origin, local_ray.direction);

            current_found = rayHitsCube(local_ray);
            if (current_found) {
                local_ray.hit.position         = xform.externPos(local_ray.hit.position);
                local_ray.hit.distance_squared = (local_ray.hit.position - ray.origin).squaredLength();
                if (local_ray.hit.distance_squared < ray.hit.distance_squared) {
                    ray.hit = local_ray.hit;
                    ray.hit.geo_type = geo->type;
                    ray.hit.geo_id = i;
                    found = true;
                }
            }
        }

        if (found) {
            ray.hit.distance = sqrtf(ray.hit.distance_squared);
            ray.hit.normal = geometries[ray.hit.geo_id].transform.externDir(ray.hit.normal).normalized();
        }

        return found;
    }
};