#ifndef _APPLICATION_H
#define _APPLICATION_H

#include "Window.hpp"

class Application
{
public:
    Window window;

    Application(uint32_t width, uint32_t height, uint32_t vwidth, uint32_t vheight, const char *title);
};

#endif