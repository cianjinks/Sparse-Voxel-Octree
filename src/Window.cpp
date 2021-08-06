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

static const Pixel colors[] = {Pixel{0, 0, 0}, Pixel{30, 30, 30},
                               Pixel{60, 60, 60}, Pixel{90, 90, 90},
                               Pixel{120, 120, 120}, Pixel{150, 150, 150},
                               Pixel{180, 180, 180}, Pixel{210, 210, 210}};

Window::Window(uint32_t width, uint32_t height, uint32_t vwidth, uint32_t vheight, const char *title)
    : _width(width), _height(height), _title(title), _vwidth(vwidth), _vheight(vheight), _vwidthf(float(vwidth)), _vheightf(float(vheight))
{
    _ratio = float(width) / float(height);
    _vratio = float(vwidth) / float(vheight);
    _pixels = _vwidth * _vheight;
    buffer = new Pixel[_pixels];
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
    ImGui::SliderFloat3("Camera Direction", glm::value_ptr(octree->CameraDir), -1.0f, 1.0f);
    ImGui::SliderFloat3("Camera Position", glm::value_ptr(octree->CameraPos), -5.0f, 5.0f);
    ImGui::SliderFloat("Camera Rotation", &octree->Rotation, 0.0f, 360.0f);
    ImGui::SliderFloat3("Octree Position", glm::value_ptr(octree->OctreeLoc), -5.0f, 5.0f);
    ImGui::SliderFloat3("Octree Color", glm::value_ptr(octree->ObjectColor), 0.0f, 1.0f);
    ImGui::SliderFloat3("Light Position", glm::value_ptr(octree->LightPos), -5.0f, 5.0f);
    ImGui::SliderFloat3("Light Color", glm::value_ptr(octree->LightColor), 0.0f, 1.0f);
    if (ImGui::Button("Refresh Octree"))
    {
        octree->DrawOctree(_vwidth, _vheight, _vwidthf, _vheightf, buffer, (float)glfwGetTime());
    }
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
}

void Window::Resize(uint32_t width, uint32_t height)
{
    glViewport(0, 0, width, height);
    _width = width;
    _height = height;
    _ratio = float(width) / float(height);
}