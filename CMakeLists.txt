cmake_minimum_required( VERSION 3.3 )

project( LinuxOpenGL )
find_package( OpenGL REQUIRED )
include_directories( ${OPENGL_INCLUDE_DIRS} )

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory( dependencies/glfw )
add_subdirectory( dependencies/glad )
add_subdirectory( dependencies/glm )

# set(source src/Main.cpp src/Window.hpp src/Window.cpp)
file(GLOB source CONFIGURE_DEPENDS "src/*.hpp" "src/*.cpp")

# ImGui
set(imgui-directory dependencies/imgui)
set(imgui-source ${imgui-directory}/imconfig.h
	${imgui-directory}/imgui.h
	${imgui-directory}/imgui.cpp
	${imgui-directory}/imgui_draw.cpp
	${imgui-directory}/imgui_internal.h
	${imgui-directory}/imgui_widgets.cpp
	${imgui-directory}/imstb_rectpack.h
	${imgui-directory}/imstb_textedit.h
	${imgui-directory}/imstb_truetype.h
    ${imgui-directory}/imgui_tables.cpp
	${imgui-directory}/imgui_demo.cpp
)

add_executable( LinuxOpenGL ${source} ${imgui-source})
target_link_libraries( LinuxOpenGL ${OPENGL_LIBRARIES} glfw glad glm)
target_include_directories(LinuxOpenGL PRIVATE ${imgui-directory})