#ifndef _WINDOW_H
#define _WINDOW_H

#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <memory>
#include <algorithm>
#include <thread>

#include "Util.hpp"
#include "Octree.hpp"

class Window
{
public:
    Window(uint32_t width, uint32_t height, uint32_t vwidth, uint32_t vheight, const char *title);

    void Setup();
    void Clear();
    void Draw(Octree *octree);
    void Exit();
    void Resize(uint32_t width, uint32_t height);

public:
    Pixel *buffer;
    float *depthBuffer;

private:
    void DrawUI(Octree *octree);
    void DrawThreaded(Octree *octree);

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

    bool _disableRefresh = false;
    bool _completedRender = false;
};

#endif