#ifndef _WINDOW_H
#define _WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "Octree.hpp"
struct Pixel
{
    uint8_t r, g, b;
};

class Window
{
public:
    Window(uint32_t width, uint32_t height, uint32_t vwidth, uint32_t vheight, const char *title);
    ~Window();

    void setup();
    void draw(Octree &octree);
    void exit();
    void resize(uint32_t width, uint32_t height);

public:
    Pixel *buffer;

private:
    void drawUI();
    // TEMP
    bool slabs(glm::vec3 &p0, glm::vec3 &p1, glm::vec3 &ro, glm::vec3 &rd, glm::vec3 &r_normal);
    Pixel shade(glm::vec3 &lightColor, glm::vec3 lightPos, glm::vec3 &objectColor, glm::vec3 &normal, glm::vec3 &hitPos);
    void drawOctree(Octree &octree);

private:
    uint32_t _width,
        _height,
        _vwidth,
        _vheight;
    float _vwidthf, _vheightf;
    float _ratio, _vratio;
    uint32_t _pixels;
    const char *_title;
    uint32_t _time;
    GLFWwindow *_window;
};

#endif