#ifndef RESULTWINDOW_H
#define RESULTWINDOW_H
#include "BaseWindow.h"
#include "implot.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <opencv2/core.hpp>
#include "utility.h"

class DLLEXPORT ResultWindow : public BaseWindow
{
private:
    // 棋盘格数据结构
    struct Grid
    {
        int rows;              // 行
        int cols;              // 列
        double size;           // 小格子边长
        glm::vec3 translation; // 平移
        glm::vec3 rotation;    // 旋转

        Grid(
            const int &_row,
            const int &_col,
            const double &_size,
            const glm::vec3 &_translation,
            const glm::vec3 &_rotation);
    };

    // 相机
    struct Camera
    {
        glm::vec3 translation; // 平移
        glm::vec3 rotation;    // 旋转
        Camera(
            const glm::vec3 &_translation,
            const glm::vec3 &_rotation);
    };
public:
    // 用来执行文本框、滑动条等基本交互，主要是用来获取关于render的信息
    virtual void run();

    // 主窗口渲染
    virtual void render();

    // 重投影
    void rendererrorbars();

    // 标定图像
    void renderCalibimgs();

    // 清除内存
    void clear();

public:
    // 初始化
    ResultWindow();
    // 添加棋盘格
    void addBoard(
        const int &_row,
        const int &_col,
        const double &_size,
        const glm::vec3 &_translation,
        const glm::vec3 &_rotation);
    // 添加相机
    void addCamera(
        const glm::vec3 &_translation,
        const glm::vec3 &_rotation);

    // 添加重投影误差
    void addlefterrors(std::vector<double> &_errors);

    void addrighterrors(std::vector<double> &_errors);

    // 单目和单目一起，注意这里双目需要拼接
    void addcalibimg(const std::vector<cv::Mat> &imgs);

private:
    // 不同类型的单次渲染
    // 棋盘格
    void rendersingle(const Grid &grid);

    // 相机
    void rendersingle(const Camera &cam);

    GLuint matToTexture(const cv::Mat &mat, GLenum minFilter, GLenum magFilter, GLenum wrapFilter);

private:
    // board grid
    std::vector<Grid> grids;
    std::vector<Camera> cams;
    std::vector<double> errors_l, errors_r;
    std::vector<GLuint> textimgs;

    std::vector<double> xs_l, xs_r;
    bool renderflag = true;
};

#endif