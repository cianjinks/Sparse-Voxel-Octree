#include <memory>
#include "Application.hpp"

int main()
{
    std::unique_ptr<Application> app = std::make_unique<Application>(950, 950, 800, 800, "SVO");
    app->Run();

    return 0;
}