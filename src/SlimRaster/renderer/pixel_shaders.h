#pragma once

#include "./common.h"
#include "../math/utils.h"
#include "../scene/scene.h"

INLINE vec3 sampleNormal(Texture &texture, vec2 uv, vec2 dUV) {
    vec3 normal = Vec3(texture.sample(uv, dUV));
    f32 y = normal.z;
    normal.z = normal.y;
    normal.y = y;
    return normal.scaleAdd(2.0f, vec3{-1.0f}).normalized();
}
INLINE quat getNormalRotation(vec3 normal, f32 magnitude) {
    quat q;
    normal *= 0.25f;
    q.axis.x = normal.z;
    q.axis.y = 0;
    q.axis.z = normal.x;
    q.amount = normal.y / magnitude;
    return q.normalized();
}

INLINE bool isChequerboard(const vec2 &UV, f32 half_step_count) {
    f32 s = UV.u * half_step_count;
    f32 t = UV.v * half_step_count;
    s -= floorf(s);
    t -= floorf(t);
    return (s > 0.5f) ^ (t < 0.5f);
}

void shadePixelTextured(Shaded &shaded, const Scene &scene) {
    shaded.color = Vec3(scene.textures[shaded.material->texture_ids[0]].sample(shaded.UV, shaded.dUV));
}

void shadePixelDepth(Shaded &shaded, const Scene &scene) {
    shaded.color = shaded.depth > 10.0f ? 1.0f : (f32)(shaded.depth * 0.1);
}

void shadePixelUV(Shaded &shaded, const Scene &scene) {
    shaded.color.r = shaded.UV.u;
    shaded.color.g = shaded.UV.v;
    shaded.color.b = 0;
}

void shadePixelPosition(Shaded &shaded, const Scene &scene) {
    shaded.color = (shaded.position + 1.0f) * 0.5f;
}

void shadePixelNormal(Shaded &shaded, const Scene &scene) {
    if (shaded.material->normal_magnitude && shaded.material->texture_count > 1)
        shaded.normal = getNormalRotation(
            sampleNormal(scene.textures[shaded.material->texture_ids[1]], shaded.UV, shaded.dUV),
            shaded.material->normal_magnitude
            ) * shaded.normal;
    shaded.color = shaded.normal.scaleAdd(0.5f, vec3{0.5f});
}

void shadePixelCheckerboard(Shaded &shaded, const Scene &scene) {
    shaded.color = isChequerboard(shaded.UV, 4) ? 1.0f : 0.0f;
}

void shadePixelClassic(Shaded &shaded, const Scene &scene) {
    MaterialUses material_uses{shaded.material->flags};
    f32 NdotL, NdotRd, squared_distance;

    shaded.diffuse = shaded.material->diffuse;
    if (shaded.material->texture_count) {
        shaded.diffuse = shaded.diffuse * Vec3(scene.textures[shaded.material->texture_ids[0]].sample(shaded.UV, shaded.dUV));
        if (shaded.material->normal_magnitude && shaded.material->texture_count > 1)
            shaded.normal = getNormalRotation(
                    sampleNormal(scene.textures[shaded.material->texture_ids[1]], shaded.UV, shaded.dUV),
                    shaded.material->normal_magnitude
            ) * shaded.normal;
    }

    shaded.color = scene.ambient_light.color;
    shaded.viewing_direction = (shaded.position - shaded.viewing_origin).normalized();
    if (material_uses.phong) {
        NdotRd = clampedValue(shaded.normal | shaded.viewing_direction);
        shaded.reflected_direction = reflectWithDot(shaded.viewing_direction, shaded.normal, NdotRd);
    }

    Light *light = scene.lights;
    for (u32 i = 0; i < scene.counts.lights; i++, light++) {
        shaded.light_direction = light->position_or_direction - shaded.position;
        squared_distance = shaded.light_direction.squaredLength();
        shaded.light_direction = shaded.light_direction / sqrtf(squared_distance);
        NdotL = shaded.normal | shaded.light_direction;
        if (NdotL > 0)
            shaded.color = shadePointOnSurface(shaded, NdotL).mulAdd(light->color * (light->intensity / squared_distance), shaded.color);
    }

    shaded.color.x = toneMappedBaked(shaded.color.x);
    shaded.color.y = toneMappedBaked(shaded.color.y);
    shaded.color.z = toneMappedBaked(shaded.color.z);
}

void shadePixelClassicChequerboard(Shaded &shaded, const Scene &scene) {
    shadePixelClassic(shaded, scene);

    if (isChequerboard(shaded.UV, 4)) shaded.color *= 0.5f;
}