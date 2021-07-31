#include "Octree.hpp"

const uint32_t Octree::BitCount[] = {
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8};

Octree::Octree()
{
    _tree = {0x0000FFFF};
}

bool Octree::raymarch(glm::vec3 &ro,
                      glm::vec3 &rd)
{
    const int s_max = 23; // Maximum scale (number of float mantissa bits).
    struct StackEntry
    {
        uint64_t offset;
        float maxT;
    };
    StackEntry rayStack[s_max + 1];

    if (std::fabs(rd.x) < 1e-4f)
        rd.x = 1e-4f;
    if (std::fabs(rd.y) < 1e-4f)
        rd.y = 1e-4f;
    if (std::fabs(rd.z) < 1e-4f)
        rd.z = 1e-4f;

    float dTx = 1.0f / -std::fabs(rd.x);
    float dTy = 1.0f / -std::fabs(rd.y);
    float dTz = 1.0f / -std::fabs(rd.z);

    float bTx = dTx * ro.x;
    float bTy = dTy * ro.y;
    float bTz = dTz * ro.z;

    uint8_t octantMask = 7;
    if (rd.x > 0.0f)
        octantMask ^= 1, bTx = 3.0f * dTx - bTx;
    if (rd.y > 0.0f)
        octantMask ^= 2, bTy = 3.0f * dTy - bTy;
    if (rd.z > 0.0f)
        octantMask ^= 4, bTz = 3.0f * dTz - bTz;

    float minT = std::max(2.0f * dTx - bTx, std::max(2.0f * dTy - bTy, 2.0f * dTz - bTz));
    float maxT = std::min(dTx - bTx, std::min(dTy - bTy, dTz - bTz));
    minT = std::max(minT, 0.0f);

    uint32_t current = 0;
    uint64_t parent = 0;
    int idx = 0;
    float posX = 1.0f;
    float posY = 1.0f;
    float posZ = 1.0f;
    int scale = s_max - 1;

    float scaleExp2 = 0.5f;

    if (1.5f * dTx - bTx > minT)
        idx ^= 1, posX = 1.5f;
    if (1.5f * dTy - bTy > minT)
        idx ^= 2, posY = 1.5f;
    if (1.5f * dTz - bTz > minT)
        idx ^= 4, posZ = 1.5f;

    while (scale < s_max)
    {
        if (current == 0)
        {
            current = _tree[parent];
        }

        float cornerTX = posX * dTx - bTx;
        float cornerTY = posY * dTy - bTy;
        float cornerTZ = posZ * dTz - bTz;
        float maxTC = std::min(cornerTX, std::min(cornerTY, cornerTZ));

        int childShift = idx ^ octantMask;
        uint32_t childMasks = current << childShift;

        if ((childMasks & 0x8000) && minT <= maxT)
        {
            if (maxTC >= scaleExp2)
            {
                return true;
            }

            float maxTV = std::min(maxT, maxTC);
            float half = scaleExp2 * 0.5f;
            float centerTX = half * dTx + cornerTX;
            float centerTY = half * dTy + cornerTY;
            float centerTZ = half * dTz + cornerTZ;

            if (minT <= maxTV)
            {
                uint64_t childOffset = current >> 16; // pointer for this parents child descriptors
                // if (current & 0x20000)
                //     childOffset = (childOffset << 32) | uint64_t(_tree[parent + 1]);

                if (!(childMasks & 0x80))
                {
                    break;
                }

                rayStack[scale].offset = parent;
                rayStack[scale].maxT = maxT;

                uint32_t siblingCount = BitCount[childMasks & 127];
                parent += childOffset + siblingCount;
                if (current & 0x10000)
                    parent += siblingCount;

                idx = 0;
                scale--;
                scaleExp2 = half;

                if (centerTX > minT)
                    idx ^= 1, posX += scaleExp2;
                if (centerTY > minT)
                    idx ^= 2, posY += scaleExp2;
                if (centerTZ > minT)
                    idx ^= 4, posZ += scaleExp2;

                maxT = maxTV;
                current = 0;

                continue;
            }
        }

        int stepMask = 0;
        if (cornerTX <= maxTC)
            stepMask ^= 1, posX -= scaleExp2;
        if (cornerTY <= maxTC)
            stepMask ^= 2, posY -= scaleExp2;
        if (cornerTZ <= maxTC)
            stepMask ^= 4, posZ -= scaleExp2;

        minT = maxTC;
        idx ^= stepMask;

        if ((idx & stepMask) != 0)
        {
            int differingBits = 0;
            if (stepMask & 1)
                differingBits |= floatBitsToUint(posX) ^ floatBitsToUint(posX + scaleExp2);
            if (stepMask & 2)
                differingBits |= floatBitsToUint(posY) ^ floatBitsToUint(posY + scaleExp2);
            if (stepMask & 4)
                differingBits |= floatBitsToUint(posZ) ^ floatBitsToUint(posZ + scaleExp2);
            scale = (floatBitsToUint((float)differingBits) >> 23) - 127;
            scaleExp2 = uintBitsToFloat((scale - s_max + 127) << 23);

            parent = rayStack[scale].offset;
            maxT = rayStack[scale].maxT;

            int shX = floatBitsToUint(posX) >> scale;
            int shY = floatBitsToUint(posY) >> scale;
            int shZ = floatBitsToUint(posZ) >> scale;
            posX = uintBitsToFloat(shX << scale);
            posY = uintBitsToFloat(shY << scale);
            posZ = uintBitsToFloat(shZ << scale);
            idx = (shX & 1) | ((shY & 1) << 1) | ((shZ & 1) << 2);

            current = 0;
        }
    }

    if (scale >= s_max)
    {
        return false;
    }

    return true;
}