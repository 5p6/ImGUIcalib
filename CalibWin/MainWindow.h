#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "imgui.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include "BaseWindow.h"
#include "CalibWindow.h"
#include "ResultWindow.h"

class DLLEXPORT MainWindow
{
public:
    // 初始化
    MainWindow(const int &_winrow, const int &_wincol);

    // 析构，清理内存
    ~MainWindow();

private:
    // 初始化 opengl 和glfw
    void init();

    // 开始渲染
    void Render();

    // 释放资源
    void release();

    // 对于主窗口的操作
    void ProcessInput();

    // 子坐标系的姿态
    void originalaxes();
private:
    /**
     * 这里有两个窗口
     * 标定窗口 calibwindow
     * 结果显示窗口 result window
     */
    std::shared_ptr<CalibWindow> calwin;
    std::shared_ptr<ResultWindow> reswin;


    // 窗口信息
    int winrow, wincol;
    GLFWwindow *window = nullptr;

    // 默认棋盘格参数
    // int defaultRows = 7;
    // int defaultCols = 6;
    // double defaultSize = 0.02;
    // glm::vec3 translation = glm::vec3(0.0f);
    // glm::vec3 rotation = glm::vec3(0.0f);

    // 相机控制相关变量
    float cameraDistance = 5.0f;
    float cameraYaw = 0.0f, cameraPitch = 0.0f;
    glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, cameraDistance);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    bool dragging = false;
    double lastMouseX, lastMouseY;
    bool panning = false;
};

#endif