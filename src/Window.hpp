#include <glad/glad.h>
#include <GLFW/glfw3.h>

#ifndef _WINDOW_H
#define _WINDOW_H

class Window
{
public:
    Window(uint32_t width, uint32_t height, uint32_t vwidth, uint32_t vheight, const char *title);
    ~Window();

    void setup();
    void draw();
    void exit();

public:
    uint8_t *buffer;

private:
    void drawUI();

private:
    uint32_t _width, _height, _vwidth, _vheight;
    uint32_t _pixels;
    const char *_title;
    uint32_t _time;
    GLFWwindow *_window;
};

#endif