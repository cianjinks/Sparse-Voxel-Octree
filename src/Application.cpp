#include "Application.hpp"

#include <iostream>

Application::Application(uint32_t width, uint32_t height, uint32_t vwidth, uint32_t vheight, const char *title)
    : window(width, height, vwidth, vheight, title)
{
    std::cout << "Created App!" << std::endl;
}

void Application::Run()
{
    std::cout << "Running App..." << std::endl;
    window.Setup();
    window.Draw(&octree);
    window.Exit();
}