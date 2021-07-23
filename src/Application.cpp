#include "Application.hpp"

Application::Application(uint32_t width, uint32_t height, uint32_t vwidth, uint32_t vheight, const char *title)
    : window(width, height, vwidth, vheight, title) {}