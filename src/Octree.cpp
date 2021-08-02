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
    // _treeSize = 1 + (8 * 9);

    // _tree = {
    //     0x0004FFFF,
    //     0x0020FFFF, 0x003CFFFF, 0x0058FFFF, 0x0074FFFF,
    //     0x0090FFFF, 0x00ACFFFF, 0x00C8FFFF, 0x00E4FFFF,

    //     0x00008200, 0x00008200, 0x00008200, 0x00008200,
    //     0x00008200, 0x00008200, 0x00008200, 0x00008200,

    //     0x00008200, 0x00008200, 0x00008200, 0x00008200,
    //     0x00008200, 0x00008200, 0x00008200, 0x00008200,

    //     0x00008200, 0x00008200, 0x00008200, 0x00008200,
    //     0x00008200, 0x00008200, 0x00008200, 0x00008200,

    //     0x00008200, 0x00008200, 0x00008200, 0x00008200,
    //     0x00008200, 0x00008200, 0x00008200, 0x00008200,

    //     0x00008200, 0x00008200, 0x00008200, 0x00008200,
    //     0x00008200, 0x00008200, 0x00008200, 0x00008200,

    //     0x00008200, 0x00008200, 0x00008200, 0x00008200,
    //     0x00008200, 0x00008200, 0x00008200, 0x00008200,

    //     0x00008200, 0x00008200, 0x00008200, 0x00008200,
    //     0x00008200, 0x00008200, 0x00008200, 0x00008200,

    //     0x00008200, 0x00008200, 0x00008200, 0x00008200,
    //     0x00008200, 0x00008200, 0x00008200, 0x00008200,
    // };
    generate();
}

void Octree::generate()
{
    uint32_t depth = 0;
    uint32_t offset = 0;
    uint32_t index = 0;

    std::stack<uint32_t> stack;
    stack.push(0);

    _tree.push_back(0x0000FFFF);
    while(!stack.empty())
    {
        offset = _tree.size() - index;
        _tree[index] |= offset << 17;
        index += offset;
        if(depth < 8)
        {
            for(int i = 0; i < 8; i++)
            {
                _tree.push_back(0x0000FFFF);
                stack.push(index + (8 - i));
            }
            depth += 8;
        }
        else
        {
            for(int i = 0; i < 8; i++) { _tree.push_back(0x00008200); }
            index = stack.top();
            stack.pop();
        }
    }

    _treeSize = _tree.size();
}

bool Octree::raymarch(glm::vec3 &ro,
                      glm::vec3 &rd,
                      glm::vec3 &r_hit,
                      uint32_t &r_normal,
                      int &r_idx)
{
    glm::vec3 rayOrigin = ro;
    glm::vec3 rayDirection = rd;

    const float rayScale = 0.0f;
    const int MaxScale = 23; // Maximum scale (number of float mantissa bits).
    struct StackEntry
    {
        uint64_t offset;
        float maxT;
    };
    StackEntry rayStack[MaxScale + 1];

    float ox = ro.x, oy = ro.y, oz = ro.z;
    float dx = rd.x, dy = rd.y, dz = rd.z;

    if (std::fabs(dx) < 1e-4f)
        dx = 1e-4f;
    if (std::fabs(dy) < 1e-4f)
        dy = 1e-4f;
    if (std::fabs(dz) < 1e-4f)
        dz = 1e-4f;

    float dTx = 1.0f / -std::fabs(dx);
    float dTy = 1.0f / -std::fabs(dy);
    float dTz = 1.0f / -std::fabs(dz);

    float bTx = dTx * ox;
    float bTy = dTy * oy;
    float bTz = dTz * oz;

    uint8_t octantMask = 7;
    if (dx > 0.0f)
        octantMask ^= 1, bTx = 3.0f * dTx - bTx;
    if (dy > 0.0f)
        octantMask ^= 2, bTy = 3.0f * dTy - bTy;
    if (dz > 0.0f)
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
    int scale = MaxScale - 1;

    float scaleExp2 = 0.5f;

    if (1.5f * dTx - bTx > minT)
        idx ^= 1, posX = 1.5f;
    if (1.5f * dTy - bTy > minT)
        idx ^= 2, posY = 1.5f;
    if (1.5f * dTz - bTz > minT)
        idx ^= 4, posZ = 1.5f;

    while (scale < MaxScale)
    {
        if (current == 0)
        {
            if(parent >= _treeSize) {
                return false;
            }
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
            if (maxTC * rayScale >= scaleExp2)
            {
                // Testing return value
                r_hit.x = std::min(std::max(ro.x + minT * rd.x, ro.x + 1e-4f), ro.x + scaleExp2 - 1e-4f);
                r_hit.y = std::min(std::max(ro.y + minT * rd.y, ro.y + 1e-4f), ro.y + scaleExp2 - 1e-4f);
                r_hit.z = std::min(std::max(ro.z + minT * rd.z, ro.z + 1e-4f), ro.z + scaleExp2 - 1e-4f);

                r_idx = idx;
                return true;
            }

            float maxTV = std::min(maxT, maxTC);
            float half = scaleExp2 * 0.5f;
            float centerTX = half * dTx + cornerTX;
            float centerTY = half * dTy + cornerTY;
            float centerTZ = half * dTz + cornerTZ;

            if (minT <= maxTV)
            {
                if (!(childMasks & 0x80))
                {
                    break;
                }

                rayStack[scale].offset = parent;
                rayStack[scale].maxT = maxT;

                uint32_t siblingCount = BitCount[childMasks & 127];
                uint64_t childOffset = current >> 17;
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
            scaleExp2 = uintBitsToFloat((scale - MaxScale + 127) << 23);

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

    if (scale >= MaxScale)
        return false;

    // Testing return value
    r_hit.x = std::min(std::max(ro.x + minT * rd.x, ro.x + 1e-4f), ro.x + scaleExp2 - 1e-4f);
    r_hit.y = std::min(std::max(ro.y + minT * rd.y, ro.y + 1e-4f), ro.y + scaleExp2 - 1e-4f);
    r_hit.z = std::min(std::max(ro.z + minT * rd.z, ro.z + 1e-4f), ro.z + scaleExp2 - 1e-4f);

    r_idx = idx;
    return true;
}