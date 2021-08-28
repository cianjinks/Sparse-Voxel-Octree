#include "Window.hpp"

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "backends/imgui_impl_opengl3.cpp"
#include "backends/imgui_impl_glfw.cpp"

#include <iostream>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <time.h>

Window::Window(uint32_t width, uint32_t height, uint32_t vwidth, uint32_t vheight, const char *title)
    : _width(width), _height(height), _title(title), _vwidth(vwidth), _vheight(vheight), _vwidthf(float(vwidth)), _vheightf(float(vheight))
{
    _ratio = float(width) / float(height);
    _vratio = float(vwidth) / float(vheight);
    _pixels = _vwidth * _vheight;
    buffer = new Pixel[_pixels];

    depthBuffer = new float[_pixels];
    std::fill_n(depthBuffer, _pixels, FLT_MAX);
}

void Window::Setup()
{
    /* Initialize the library */
    if (!glfwInit())
    {
        assert(false);
    }

    /* Create a windowed mode window and its OpenGL context */
    _window = glfwCreateWindow(_width, _height, _title, NULL, NULL);
    if (!_window)
    {
        glfwTerminate();
        assert(false);
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(_window);
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    if (!status)
    {
        assert(false);
    }

    glfwSetWindowUserPointer(_window, this);
    glfwSetFramebufferSizeCallback(_window, [](GLFWwindow *window, int width, int height)
                                   {
                                       Window *w = (Window *)glfwGetWindowUserPointer(window);
                                       w->Resize((uint32_t)width, (uint32_t)height);
                                   });

    glEnable(GL_DEBUG_OUTPUT);

    float vertices[] = {
        // Pos // Texture Coord
        0.0f, _vheightf, 0.0f, 0.0f, 1.0f,     // Top Left
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f,          // Bottom Left
        _vwidthf, 0.0f, 0.0f, 1.0f, 0.0f,      // Bottom Right
        _vwidthf, _vheightf, 0.0f, 1.0f, 1.0f, // Top Right
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0};

    GLuint vaoID;
    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    GLuint vboID;
    glGenBuffers(1, &vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(float), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    GLuint iboID;
    glGenBuffers(1, &iboID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    GLuint programID;
    programID = glCreateProgram();

    std::string vertexShader, line;
    std::ifstream invert("../src/shaders/vert.glsl");
    while (std::getline(invert, line))
    {
        vertexShader += line + '\n';
    }
    const char *vertexShaderSource = vertexShader.c_str();

    GLuint vertexShaderObj;
    vertexShaderObj = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderObj, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShaderObj);
    glAttachShader(programID, vertexShaderObj);
    glDeleteShader(vertexShaderObj);

    std::string fragmentShader;
    std::ifstream infrag("../src/shaders/frag.glsl");
    while (std::getline(infrag, line))
    {
        fragmentShader += line + '\n';
    }
    const char *fragmentShaderSource = fragmentShader.c_str();

    GLuint fragmentShaderObj;
    fragmentShaderObj = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderObj, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShaderObj);
    glAttachShader(programID, fragmentShaderObj);
    glDeleteShader(fragmentShaderObj);

    glLinkProgram(programID);
    glValidateProgram(programID);
    glUseProgram(programID);

    glm::mat4 ortho = glm::ortho(0.0f, _vwidthf, 0.0f, _vheightf, -1.0f, 1.0f);

    GLint loc = glGetUniformLocation(programID, "u_ProjectionMatrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(ortho));

    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(_window, true);
    ImGui_ImplOpenGL3_Init("#version 410");
}

void Window::DrawUI(Octree *octree)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // ImGui::ShowDemoWindow();
    // ImGui::Begin("Test");
    // ImGui::End();

    ImGui::Begin("Options");

    ImGui::Text("General");
    ImGui::SliderFloat3("Camera Direction", glm::value_ptr(octree->CameraDir), -1.0f, 1.0f);
    ImGui::SliderFloat3("Camera Position", glm::value_ptr(octree->CameraPos), -5.0f, 5.0f);
    ImGui::SliderFloat("Camera Rotation", &octree->Rotation, 0.0f, 360.0f);
    ImGui::SliderFloat3("Octree Position", glm::value_ptr(octree->OctreeLoc), -5.0f, 5.0f);
    ImGui::SliderFloat3("Octree Color", glm::value_ptr(octree->ObjectColor), 0.0f, 1.0f);
    ImGui::SliderFloat3("Light Position", glm::value_ptr(octree->LightPos), -5.0f, 5.0f);
    ImGui::SliderFloat("Light Size", &octree->LightSize, 0.0f, 2.0f);
    ImGui::SliderFloat3("Light Color", glm::value_ptr(octree->LightColor), 0.0f, 1.0f);

    ImGui::Text("Path Tracer");
    ImGui::SliderInt("Samples", &octree->NumSamples, 0, 250);
    ImGui::SliderInt("Max Bounces", &octree->MaxBounces, 0, 100);
    ImGui::SliderFloat("Reflectivity", &octree->Reflectivity, 0.0f, 1.0f);

    // Rendering
    if (ImGui::Button("Refresh"))
    {
        if (!_disableRefresh)
        {
            _disableRefresh = true;
            std::thread t(&Window::DrawThreaded, this, octree);
            t.detach();
        }
    }

    if (_disableRefresh)
    {
        ImGui::SameLine();
        ImGui::Text("Rendering...");
    }
    else
    {
        ImGui::SameLine();
        ImGui::Text("Complete!");
    }

    ImGui::Separator();

    ImGui::Text("Shading Mode");
    if (ImGui::Button("Depth"))
    {
        octree->ShadingMode = Shade::DEPTH;
    }
    ImGui::SameLine();
    if (ImGui::Button("Depth Hit"))
    {
        octree->ShadingMode = Shade::DEPTH_HIT;
    }
    ImGui::SameLine();
    if (ImGui::Button("Diffuse"))
    {
        octree->ShadingMode = Shade::DIFFUSE;
    }
    ImGui::SameLine();
    if (ImGui::Button("Normal"))
    {
        octree->ShadingMode = Shade::NORMAL;
    }
    ImGui::SameLine();
    if (ImGui::Button("Index"))
    {
        octree->ShadingMode = Shade::INDEX;
    }
    ImGui::SameLine();
    if (ImGui::Button("Trace"))
    {
        octree->ShadingMode = Shade::TRACE;
    }

    ImGui::Text("Projection");
    if (ImGui::Button("Perspective"))
    {
        octree->ProjectionMode = Projection::PERSPECTIVE;
    }
    ImGui::SameLine();
    if (ImGui::Button("Orthographic"))
    {
        octree->ProjectionMode = Projection::ORTHOGRAPHIC;
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Window::DrawThreaded(Octree *octree)
{
    octree->DrawOctree(_vwidth, _vheight, _vwidthf, _vheightf, buffer, depthBuffer, (float)glfwGetTime());
    _disableRefresh = false;
}

void Window::Draw(Octree *octree)
{
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(_window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

        // octree->DrawOctree(_vwidth, _vheight, _vwidthf, _vheightf, buffer, (float)glfwGetTime());

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, _vwidth, _vheight, 0, GL_RGB, GL_UNSIGNED_BYTE, (uint8_t *)buffer);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        DrawUI(octree);

        /* Swap front and back buffers */
        glfwSwapBuffers(_window);

        /* Poll for and process events */
        glfwPollEvents();
    }
}

void Window::Exit()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();

    delete[] buffer;
    delete[] depthBuffer;
}

void Window::Resize(uint32_t width, uint32_t height)
{
    glViewport(0, 0, width, height);
    _width = width;
    _height = height;
    _ratio = float(width) / float(height);
}