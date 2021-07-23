#include <memory>
#include "Application.hpp"

int main()
{
    std::unique_ptr<Application> app = std::make_unique<Application>(1280, 720, 256, 144, "SVO");
    app->window.setup();
    app->window.draw();
    app->window.exit();

    return 0;
}