#ifndef _OCTREE_H
#define _OCTREE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <cstdint>
#include <iostream>

class Octree
{
public:
    Octree();

    bool raymarch(glm::vec3 &ro,
                  glm::vec3 &rd,
                  glm::vec3 &r_hit,
                  int &r_idx);

private:
    std::vector<uint32_t> _tree;
    static const uint32_t BitCount[];

    static inline float
    uintBitsToFloat(uint32_t i)
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
};

#endif