#include "MainWindow.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "implot.h"
#include "implot.h"
#include <vector>

MainWindow::MainWindow(const int &_winrow, const int &_wincol) : winrow(_winrow), wincol(_wincol)
{
    calwin = std::make_shared<CalibWindow>(int(winrow / 3), 450, 0, wincol - 450);
    reswin = std::make_shared<ResultWindow>();
    // 数据连接
    calwin->setResultwindow(reswin);

    // 初始化
    init();

    // 渲染
    Render();
}

MainWindow::~MainWindow()
{
    // 释放资源
    release();
}
void MainWindow::ProcessInput()
{
    // 检查是否悬停在ImGui窗口上方，如果是则不处理相机控制
    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) || ImGui::IsAnyItemActive())
    {
        return; // 如果鼠标在ImGui窗口上，阻止相机的平移和旋转
    }

    // 鼠标左键控制旋转
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        if (!dragging)
        {
            dragging = true;
            glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
        }
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        float deltaX = static_cast<float>(mouseX - lastMouseX);
        float deltaY = static_cast<float>(mouseY - lastMouseY);
        cameraYaw += deltaX * 0.01f;
        cameraPitch += deltaY * 0.01f;

        lastMouseX = mouseX;
        lastMouseY = mouseY;
    }
    else
    {
        dragging = false;
    }

    // 鼠标右键控制平移
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        if (!panning)
        {
            panning = true;
            glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
        }
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        float deltaX = static_cast<float>(mouseX - lastMouseX);
        float deltaY = static_cast<float>(mouseY - lastMouseY);

        float panSpeed = 0.005f;
        glm::vec3 right = glm::normalize(glm::cross(cameraTarget - cameraPosition, glm::vec3(0.0f, 1.0f, 0.0f)));
        glm::vec3 up = glm::normalize(glm::cross(right, cameraTarget - cameraPosition));

        cameraPosition += -right * deltaX * panSpeed;
        cameraTarget += -right * deltaX * panSpeed;
        cameraPosition += up * deltaY * panSpeed;
        cameraTarget += up * deltaY * panSpeed;

        lastMouseX = mouseX;
        lastMouseY = mouseY;
    }
    else
    {
        panning = false;
    }

    // 鼠标滚轮控制缩放
    float scroll = ImGui::GetIO().MouseWheel;
    if (scroll != 0)
        std::cout << "scroll : " << scroll << std::endl;
    // cameraPosition -= scroll * 0.5f;
    if (cameraDistance < 1.0f)
        cameraDistance = 1.0f;

    if (cameraDistance < 1.0f)
        cameraDistance = 1.0f;
    cameraPosition = glm::vec3(cameraDistance * cos(cameraYaw) * cos(cameraPitch),
                               cameraDistance * sin(cameraPitch),
                               cameraDistance * sin(cameraYaw) * cos(cameraPitch));
    cameraPosition = glm::normalize(cameraPosition - cameraTarget) * cameraDistance + cameraTarget;
}

// 子坐标系的姿态
void MainWindow::originalaxes()
{
    ImGui::Begin("orginal axes");

    ImGui::End();
}

void MainWindow::init()
{
    if (!glfwInit())
    {
        throw std::runtime_error("GLFW initialization failed");
    }

    window = glfwCreateWindow(wincol, winrow, "Main Window", NULL, NULL);
    if (!window)
    {
        std::cerr << "GLFW window creation failed" << std::endl;
        glfwTerminate();
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    glEnable(GL_DEPTH_TEST);
}
void MainWindow::Render()
{
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        // 设置主窗口旋转、平移、缩放
        ProcessInput();

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPosition, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(glm::value_ptr(projection));

        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(glm::value_ptr(view));

        // 主窗口渲染，需要在这里就开始
        // calwin->render();
        reswin->render();

        // 新的帧
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        // 运行
        calwin->run();
        // reswin->run();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
}

void MainWindow::release()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    ImPlot::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}