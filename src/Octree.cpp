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

static const Pixel colors[] = {Pixel{0, 0, 0}, Pixel{30, 30, 30},
                               Pixel{60, 60, 60}, Pixel{90, 90, 90},
                               Pixel{120, 120, 120}, Pixel{150, 150, 150},
                               Pixel{180, 180, 180}, Pixel{210, 210, 210}};

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

    Generate();

    // _treeSize = 1;
    // _tree = {0x00001F00};
}

void Octree::Generate()
{
    const uint32_t maxDepth = 3;

    uint32_t depth = 0;
    uint32_t offset = 0;
    uint32_t index = 0;

    struct StackEntry
    {
        uint32_t index;
        uint32_t depth;
    };
    std::stack<StackEntry> stack;
    stack.push(StackEntry{0, 0});
    StackEntry current;

    _tree.push_back(0x0000FFFF);
    while (!stack.empty())
    {
        offset = _tree.size() - index;
        _tree[index] |= offset << 17;
        index += offset;
        if (depth < maxDepth)
        {
            depth++;
            for (int i = 0; i < 8; i++)
            {
                _tree.push_back(0x0000FFFF);
                stack.push(StackEntry{index + (8 - i), depth});
            }
        }
        else
        {
            for (int i = 0; i < 8; i++)
            {
                _tree.push_back(0x00004200);
            }
            current = stack.top();
            index = current.index;
            depth = current.depth;
            stack.pop();
        }
    }

    _treeSize = _tree.size();
}

bool Octree::Raymarch(glm::vec3 &ro,
                      glm::vec3 &rd,
                      glm::vec3 &r_hit,
                      glm::vec3 &r_normal,
                      int &r_idx,
                      float &r_depth)
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
            if (parent >= _treeSize)
            {
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
#if 0
                // Testing return value
                // Hit
                // r_hit.x = ro.x + (minT * rd.x);
                // r_hit.y = ro.x + (minT * rd.x);
                // r_hit.z = ro.x + (minT * rd.x);
                r_hit.x = std::min(std::max(ro.x + minT * rd.x, posX + 1e-4f), posX + scaleExp2 - 1e-4f);
                r_hit.y = std::min(std::max(ro.y + minT * rd.y, posY + 1e-4f), posY + scaleExp2 - 1e-4f);
                r_hit.z = std::min(std::max(ro.z + minT * rd.z, posZ + 1e-4f), posZ + scaleExp2 - 1e-4f);

                // Normal
                glm::vec3 t_corner;
                t_corner.x = dTx * (posX + scaleExp2) - bTx;
                t_corner.y = dTy * (posY + scaleExp2) - bTy;
                t_corner.z = dTz * (posZ + scaleExp2) - bTz;
                if (t_corner.x > t_corner.y && t_corner.x > t_corner.z)
                {
                    r_normal = glm::vec3(-1, 0, 0);
                }
                else if (t_corner.y > t_corner.z)
                {
                    r_normal = glm::vec3(0, -1, 0);
                }
                else
                {
                    r_normal = glm::vec3(0, 0, -1);
                }
                if ((octantMask & 1u) == 0u)
                    r_normal.x = -r_normal.x;
                if ((octantMask & 2u) == 0u)
                    r_normal.y = -r_normal.y;
                if ((octantMask & 4u) == 0u)
                    r_normal.z = -r_normal.z;
                // r_normal = glm::normalize(r_normal);

                // Depth
                r_depth = minT;

                // Index
                r_idx = idx ^ octantMask ^ 7;
                return true;
#endif
                break;
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
    // Hit
    r_hit.x = ro.x + (minT * rd.x);
    r_hit.y = ro.y + (minT * rd.y);
    r_hit.z = ro.z + (minT * rd.z);
    // r_hit.x = std::min(std::max(ro.x + (minT * rd.x), posX + 1e-4f), posX + scaleExp2 - 1e-4f);
    // r_hit.y = std::min(std::max(ro.y + (minT * rd.y), posY + 1e-4f), posY + scaleExp2 - 1e-4f);
    // r_hit.z = std::min(std::max(ro.z + (minT * rd.z), posZ + 1e-4f), posZ + scaleExp2 - 1e-4f);

    // Normal
    glm::vec3 t_corner;
    t_corner.x = dTx * (posX + scaleExp2) - bTx;
    t_corner.y = dTy * (posY + scaleExp2) - bTy;
    t_corner.z = dTz * (posZ + scaleExp2) - bTz;
    if (t_corner.x > t_corner.y && t_corner.x > t_corner.z)
    {
        r_normal = glm::vec3(-1, 0, 0);
    }
    else if (t_corner.y > t_corner.z)
    {
        r_normal = glm::vec3(0, -1, 0);
    }
    else
    {
        r_normal = glm::vec3(0, 0, -1);
    }
    if ((octantMask & 1u) == 0u)
        r_normal.x = -r_normal.x;
    if ((octantMask & 2u) == 0u)
        r_normal.y = -r_normal.y;
    if ((octantMask & 4u) == 0u)
        r_normal.z = -r_normal.z;
    // r_normal = glm::normalize(r_normal);

    // Depth
    r_depth = minT;

    // Index
    r_idx = idx ^ octantMask ^ 7;
    return true;
}

void Octree::DrawOctree(uint32_t vwidth, uint32_t vheight, float vwidthf, float vheightf, Pixel *buffer, float *depthBuffer, float time)
{
    // Light
    glm::vec3 p0 = LightPos - LightSize;
    glm::vec3 p1 = LightPos + LightSize;

    // Rotation
    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::rotate(trans, glm::radians(Rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    // trans = glm::rotate(trans, (float)time, glm::vec3(0.0f, 1.0f, 0.0f));

    glm::vec3 cameraPos = glm::vec3(trans * glm::vec4(CameraPos, 1.0f));
    glm::vec3 cameraDir = glm::vec3(trans * glm::vec4(CameraDir, 1.0f));

    float rx, ry;
    glm::vec3 pos;
    glm::vec3 octreeRayOrigin;
    glm::vec3 rayOrigin;
    glm::vec3 rayDirection;

    glm::vec3 hit;
    glm::vec3 normal;
    float depth = 0;
    int idx = 0;

    uint64_t index = 0;
    for (int y = 0; y < vheight; y++)
    {
        for (int x = 0; x < vwidth; x++)
        {
            rx = ((float(x) * 2.0f) / vwidthf) - 1.0f;
            ry = ((float(y) * 2.0f) / vheightf) - 1.0f;
            pos = glm::vec3(rx, 0.0f, ry);
            pos = glm::vec3(trans * glm::vec4(pos, 1.0f));

            // Ray
            if (ProjectionMode == Projection::PERSPECTIVE)
            {
                // Perspective:
                rayOrigin = cameraPos;
                octreeRayOrigin = cameraPos - OctreeLoc;
                rayDirection = cameraDir + pos;
            }
            else if (ProjectionMode == Projection::ORTHOGRAPHIC)
            {
                // Orthographic:
                rayOrigin = cameraPos + pos;
                octreeRayOrigin = cameraPos + pos - OctreeLoc;
                rayDirection = cameraDir;
            }

            index = x + (y * vwidth);

            if (ShadingMode == Shade::TRACE)
            {
                if (Raymarch(octreeRayOrigin, rayDirection, hit, normal, idx, depth))
                {
                    buffer[index] = Pixel{0, 255, 0};
                }
                else if (rayAABB(FloorCorner0, FloorCorner1, rayOrigin, rayDirection))
                {
                    buffer[index] = Pixel{100, 100, 100};
                }
                else
                {
                    buffer[index] = Pixel{25, 25, 25};
                }
            }
            else
            {
                if (Raymarch(octreeRayOrigin, rayDirection, hit, normal, idx, depth))
                {
                    hit += OctreeLoc;
                    depthBuffer[index] = depth;
                    switch (ShadingMode)
                    {
                    case Shade::DEPTH:
                        buffer[index] = ShadeDepth(ObjectColor, depth);
                        break;
                    case Shade::DEPTH_HIT:
                        buffer[index] = ShadeDepthFromHit(ObjectColor, cameraPos, hit);
                        break;
                    case Shade::DIFFUSE:
                        buffer[index] = ShadeDiffuse(cameraPos, LightColor, LightPos, ObjectColor, normal, hit);
                        break;
                    case Shade::NORMAL:
                        buffer[index] = ShadeNormal(normal);
                        break;
                    case Shade::INDEX:
                        buffer[index] = colors[idx];
                        break;
                    default:
                        buffer[index] = Pixel{0, 0, 255};
                    }
                }
                else if (rayAABB(p0, p1, rayOrigin, rayDirection))
                {
                    buffer[index].r = (uint32_t)(LightColor.r * 255.0f);
                    buffer[index].g = (uint32_t)(LightColor.g * 255.0f);
                    buffer[index].b = (uint32_t)(LightColor.b * 255.0f);
                }
                else
                {
                    buffer[index].r = 25;
                    buffer[index].g = 25;
                    buffer[index].b = 25;
                }
            }
        }
    }
}

Pixel Octree::ShadeDiffuse(glm::vec3 &cameraPos, glm::vec3 &lightColor, glm::vec3 lightPos, glm::vec3 &objectColor, glm::vec3 &normal, glm::vec3 &hitPos)
{
    // Ambient
    float ambientStrength = 0.1f;
    glm::vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    glm::vec3 lightDir = glm::normalize(lightPos - hitPos);
    float diff = std::max(glm::dot(normal, lightDir), 0.0f);
    glm::vec3 diffuse = diff * lightColor;

    // Specular
    // float specularStrength = 0.5;
    // glm::vec3 reflectDir = glm::reflect(-lightDir, normal);
    // float spec = glm::pow(std::max(glm::dot(viewDir, reflectDir), 0.0f), 32);
    // glm::vec3 specular = specularStrength * spec * lightColor;

    glm::vec3 resultv = (ambient + diffuse) * objectColor;

    Pixel result;
    result.r = (uint32_t)(resultv.r * 255.0f);
    result.g = (uint32_t)(resultv.g * 255.0f);
    result.b = (uint32_t)(resultv.b * 255.0f);
    return result;
}

Pixel Octree::ShadeDepth(glm::vec3 &objectColor, float &depth)
{
    Pixel result;
    result.r = (uint32_t)(objectColor.r * 255.0f);
    result.g = (uint32_t)(objectColor.g * 255.0f * (depth / 2.0f));
    result.b = (uint32_t)(objectColor.b * 255.0f);
    return result;
}

Pixel Octree::ShadeDepthFromHit(glm::vec3 &objectColor, glm::vec3 &cameraPos, glm::vec3 &hit)
{
    float depth = glm::distance(cameraPos, hit);

    Pixel result;
    result.r = (uint32_t)(objectColor.r * 255.0f);
    result.g = (uint32_t)(objectColor.g * 255.0f * (depth / 2.0f));
    result.b = (uint32_t)(objectColor.b * 255.0f);
    return result;
}

Pixel Octree::ShadeNormal(glm::vec3 &normal)
{
    Pixel result;
    result.r = (normal.x != 0) ? 255 : 0;
    result.g = (normal.y != 0) ? 255 : 0;
    result.b = (normal.z != 0) ? 255 : 0;
    return result;
}

bool Octree::rayAABB(glm::vec3 &p0, glm::vec3 &p1, glm::vec3 &ro, glm::vec3 &rd)
{
    glm::vec3 invRd = 1.0f / rd;
    glm::vec3 t0 = (p0 - ro) * invRd;
    glm::vec3 t1 = (p1 - ro) * invRd;
    glm::vec3 tmin = glm::min(t0, t1);
    glm::vec3 tmax = glm::max(t0, t1);

    return std::max(std::max(tmin.x, tmin.y), tmin.z) <= std::min(std::min(tmax.x, tmax.y), tmax.z);
}

bool Octree::rayPlane(const glm::vec3 &n, const glm::vec3 &pos, const glm::vec3 &ro, const glm::vec3 &rd, float &t)
{
    float denom = glm::dot(n, rd);
    if (denom > 1e-6)
    {
        glm::vec3 v = pos - ro;
        t = glm::dot(v, n) / denom;
        return (t >= 0);
    }

    return false;
}