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
}

Window::~Window()
{
}

void Window::setup()
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
                                       w->resize((uint32_t)width, (uint32_t)height);
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

void Window::drawUI()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // ImGui::ShowDemoWindow();
    // ImGui::Begin("Test");
    // ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Window::draw(Octree &octree)
{
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(_window))
    {
        drawOctree(octree);

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        drawUI();

        /* Swap front and back buffers */
        glfwSwapBuffers(_window);

        /* Poll for and process events */
        glfwPollEvents();
    }
}

void Window::exit()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();

    delete[] buffer;
}

void Window::resize(uint32_t width, uint32_t height)
{
    glViewport(0, 0, width, height);
    _width = width;
    _height = height;
    _ratio = float(width) / float(height);
}

#if 1
void Window::drawOctree(Octree &octree)
{

    // glm::vec3 ro = glm::vec3(1.0f, 1.0f, -1.0f);
    // glm::vec3 rd = glm::vec3(0.0f, 0.0f, 1.0f);
    // bool test = octree.raymarch(ro, rd);
    // printf("Test: %d\n", test);

    double time = glfwGetTime();

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -2.0f);
    glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 1.0f);

    float rx, ry;
    glm::vec3 pos;
    glm::vec3 rayOrigin;
    glm::vec3 rayDirection;

    uint64_t index = 0;
    for (int y = 0; y < _vheight; y++)
    {
        for (int x = 0; x < _vwidth; x++)
        {
            index = x + (y * _vwidth);
            // rx = ((float(x) * 2.0f) / _vwidthf) - 1.0f;
            // ry = ((float(y) * 2.0f) / _vheightf) - 1.0f;
            rx = (float(x) / _vwidthf);
            ry = (float(y) / _vheightf);
            pos = glm::vec3(rx, ry, 0.0f);

            // Ray
            rayOrigin = cameraPos;
            rayDirection = glm::normalize(cameraDirection + pos);

            if (octree.raymarch(rayOrigin, rayDirection))
            {
                buffer[index].r = 255;
                buffer[index].g = 0;
                buffer[index].b = 0;
            }
            else
            {
                buffer[index].r = 0;
                buffer[index].g = 255;
                buffer[index].b = 255;
            }
        }
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, _vwidth, _vheight, 0, GL_RGB, GL_UNSIGNED_BYTE, (uint8_t *)buffer);
}
#endif

bool Window::slabs(glm::vec3 &p0, glm::vec3 &p1, glm::vec3 &ro, glm::vec3 &rd, glm::vec3 &r_normal)
{
    glm::vec3 invRd = 1.0f / rd;
    glm::vec3 t0 = (p0 - ro) * invRd;
    glm::vec3 t1 = (p1 - ro) * invRd;
    glm::vec3 tmin = glm::min(t0, t1);
    glm::vec3 tmax = glm::max(t0, t1);

    glm::vec3 tminyzx = glm::vec3(tmin.y, tmin.z, tmin.x);
    glm::vec3 tminzxy = glm::vec3(tmin.z, tmin.x, tmin.y);

    r_normal = -glm::sign(rd) * glm::step(tminyzx, tmin) * glm::step(tminzxy, tmin);

    return std::max(std::max(tmin.x, tmin.y), tmin.z) <= std::min(std::min(tmax.x, tmax.y), tmax.z);
}