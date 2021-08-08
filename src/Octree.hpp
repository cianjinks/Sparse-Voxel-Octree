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

enum Shade
{
    DEPTH = 0,
    DEPTH_HIT,
    DIFFUSE,
    NORMAL,
    INDEX,
    TRACE
};

enum Projection
{
    PERSPECTIVE = 0,
    ORTHOGRAPHIC
};

class Octree
{
public:
    // Settings
    Shade ShadingMode = Shade::TRACE;
    Projection ProjectionMode = Projection::PERSPECTIVE;

    // Camera
    glm::vec3 OctreeLoc = glm::vec3(-1.5f, -1.5f, -1.5f); // Octree Location
    glm::vec3 CameraPos = glm::vec3(0.0f, -2.0f, 0.5f);
    glm::vec3 CameraDir = glm::vec3(0.0f, 1.0f, -0.4f);

    // Lighting
    glm::vec3 LightColor = glm::vec3(1.0f);
    glm::vec3 LightPos = glm::vec3(0.0f, -1.5f, 0.0f);
    float LightSize = 0.05f;
    glm::vec3 ObjectColor = glm::vec3(0.0f, 1.0f, 0.0f);

    // Floor
    glm::vec3 FloorCorner0 = glm::vec3(1.5f, 1.5f, -0.5f);
    glm::vec3 FloorCorner1 = glm::vec3(-1.5f, -1.5f, -1.0f);
    glm::vec3 PlaneNormal = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 PlaneDistance = glm::vec3(0.0f, 0.0f, 0.0f);

    // Rotation
    float Rotation = 120.0f;

    // Path Tracing
    int MaxDepth = 50;

    Octree();

    void Generate();

    void DrawOctree(uint32_t vwidth, uint32_t vheight, float vwidthf, float vheightf, Pixel *buffer, float *depthBuffer, float time);

    bool Raymarch(glm::vec3 &rayOrigin,
                  glm::vec3 &rayDirection,
                  glm::vec3 &r_hit,
                  glm::vec3 &r_normal,
                  int *r_idx,
                  float *r_depth);

private:
    std::vector<uint32_t> _tree;
    uint64_t _treeSize;
    static const uint32_t BitCount[];

    Pixel ShadeDiffuse(glm::vec3 &cameraPos, glm::vec3 &lightColor, glm::vec3 lightPos, glm::vec3 &objectColor, glm::vec3 &normal, glm::vec3 &hitPos);
    Pixel ShadeDepth(glm::vec3 &objectColor, float &depth);
    Pixel ShadeDepthFromHit(glm::vec3 &objectColor, glm::vec3 &cameraPos, glm::vec3 &hit);
    Pixel ShadeNormal(glm::vec3 &normal);
    glm::vec3 ShadePathTrace(glm::vec3 ro, glm::vec3 rd, glm::vec3 hit, glm::vec3 normal, int depth);

    bool rayAABB(glm::vec3 &p0, glm::vec3 &p1, glm::vec3 &ro, glm::vec3 &rd, glm::vec3 &hit, glm::vec3 &normal);
    /* Assumes all vectors are normalized. `dist` is the plane distance from the origin. */
    bool rayPlane(glm::vec3 &n, glm::vec3 &dist, glm::vec3 &ro, glm::vec3 &rd, glm::vec3 &hit);
};

#endif