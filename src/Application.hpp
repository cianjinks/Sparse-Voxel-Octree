#ifndef _APPLICATION_H
#define _APPLICATION_H

#include <memory>

#include "Window.hpp"
#include "Octree.hpp"
class Application
{
public:
    Window window;
    Octree octree;

    Application(uint32_t width, uint32_t height, uint32_t vwidth, uint32_t vheight, const char *title);

    void Run();
};

#endif