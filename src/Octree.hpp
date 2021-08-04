#ifndef _OCTREE_H
#define _OCTREE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <stack>
#include <cstdint>
#include <iostream>

#include "Util.hpp"

class Octree
{
public:
    Octree();

    void Generate();

    void DrawOctree(uint32_t vwidth, uint32_t vheight, float vwidthf, float vheightf, Pixel *buffer, float time);

    bool Raymarch(glm::vec3 &ro,
                  glm::vec3 &rd,
                  glm::vec3 &r_hit,
                  glm::vec3 &r_normal,
                  int &r_idx,
                  float &r_depth);

private:
    std::vector<uint32_t> _tree;
    uint64_t _treeSize;
    static const uint32_t BitCount[];

    Pixel Shade(glm::vec3 &cameraPos, glm::vec3 &lightColor, glm::vec3 lightPos, glm::vec3 &objectColor, glm::vec3 &normal, glm::vec3 &hitPos);
    Pixel ShadeDepth(glm::vec3 &objectColor, float &depth);

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
};

#endif