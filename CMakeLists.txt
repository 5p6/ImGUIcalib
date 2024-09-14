cmake_minimum_required(VERSION 3.20)



project(test)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

# 依赖库
find_package(OpenGL)
find_package(glfw3)
find_package(glm CONFIG REQUIRED)
find_package(OpenCV REQUIRED)
find_package(spdlog)

# imgui的源代码
aux_source_directory(${PROJECT_SOURCE_DIR}/imgui/src IMGUI)
set(ImGuiSource
    ${IMGUI}
    # imgui file
    ${PROJECT_SOURCE_DIR}/imgui/extension/imguifile/ImGuiFileDialog.cpp
    # ${PROJECT_SOURCE_DIR}/imgui/extension/imguifile/dirent.h
    # imgui plot
    ${PROJECT_SOURCE_DIR}/imgui/extension/implot/implot_internal.h
    ${PROJECT_SOURCE_DIR}/imgui/extension/implot/implot.h
    ${PROJECT_SOURCE_DIR}/imgui/extension/implot/implot_demo.cpp
    ${PROJECT_SOURCE_DIR}/imgui/extension/implot/implot_items.cpp
    ${PROJECT_SOURCE_DIR}/imgui/extension/implot/implot.cpp
)

# 头文件路径
include_directories(
    ${PROJECT_SOURCE_DIR}/imgui/extension/imguifile
    ${PROJECT_SOURCE_DIR}/imgui/extension/implot
    ${PROJECT_SOURCE_DIR}/imgui/src
    ${PROJECT_SOURCE_DIR}/CalibWin
    ${PROJECT_SOURCE_DIR}
    ${OpenCV_INCLUDE_DIRS}
)

# 静态链接库
add_library(CALIBWIN SHARED
    ${ImGuiSource}
    ${PROJECT_SOURCE_DIR}/CalibWin/MainWindow.cc
    ${PROJECT_SOURCE_DIR}/CalibWin/CalibWindow.cc
    ${PROJECT_SOURCE_DIR}/CalibWin/ResultWindow.cc
    ${PROJECT_SOURCE_DIR}/CalibWin/utility.cc
)
target_link_libraries(CALIBWIN PRIVATE
    OpenGL::GL
    glfw
    ${OpenCV_LIBS}
)



# 示例程序
add_executable(test test.cc)
target_link_libraries(test
    CALIBWIN
    OpenGL::GL
    glfw
    glm::glm
)