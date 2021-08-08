#ifndef _UTIL_H
#define _UTIL_H

#include <glm/glm.hpp>

#include <cstdint>
#include <cstdlib>
#include <iostream>

struct Pixel
{
    uint8_t r, g, b;
};

static inline float uintBitsToFloat(uint32_t i)
{
    union
    {
        uint32_t i;
        float f;
    } unionHack;
    unionHack.i = i;
    return unionHack.f;
}

static inline uint32_t floatBitsToUint(float f)
{
    union
    {
        uint32_t i;
        float f;
    } unionHack;
    unionHack.f = f;
    return unionHack.i;
}

inline float random_float()
{
    // Returns a random real in [0,1).
    return rand() / (RAND_MAX + 1.0);
}

inline float random_float(float min, float max)
{
    // Returns a random real in [min,max).
    return min + (max - min) * random_float();
}

inline static glm::vec3 random(float min, float max)
{
    return glm::vec3(random_float(min, max), random_float(min, max), random_float(min, max));
}

static glm::vec3 random_in_unit_sphere()
{
    while (true)
    {
        auto p = random(-1.0f, 1.0f);
        auto plength = glm::length(p);
        if ((plength * plength) >= 1.0f)
            continue;
        return p;
    }
}

static glm::vec3 random_in_hemisphere(glm::vec3 &normal)
{
    glm::vec3 in_unit_sphere = random_in_unit_sphere();
    if (dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
        return in_unit_sphere;
    else
        return -in_unit_sphere;
}

#endif