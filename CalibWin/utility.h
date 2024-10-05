#ifndef UTILITYMY_H
#define UTILITYMY_H
#include <GLFW/glfw3.h>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
#include "imgui.h"
#include "ImGuiFileDialog.h"
#include <iostream>
#include "BaseWindow.h"

namespace utility
{
    // 将opencv的mat转换为opengl可接受的数据类型
    DLLEXPORT GLuint matToTexture(const cv::Mat &mat);

    
    // imgui 对文件夹路径选择的支持
    DLLEXPORT void SelectFolderButton(
        std::string &filepath,
        const char *button,
        const char *dialogKey,
        const char *title);

    // 拼接两张图片
    DLLEXPORT bool concat(const cv::Mat& left,const cv::Mat& right,cv::Mat& dst);
}

#endif