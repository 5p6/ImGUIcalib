#ifndef UTILITYMY_H
#define UTILITYMY_H
#include <GLFW/glfw3.h>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
#include "imgui.h"
#include "ImGuiFileDialog.h"
#include <iostream>

namespace utility
{
    // 将opencv的mat转换为opengl可接受的数据类型
    GLuint matToTexture(const cv::Mat &mat);

    
    // imgui 对文件夹路径选择的支持
    void SelectFolderButton(
        std::string &filepath,
        const char *button,
        const char *dialogKey,
        const char *title);

    //
    bool concat(const cv::Mat& left,const cv::Mat& right,cv::Mat& dst);
}

#endif