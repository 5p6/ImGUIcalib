#include "utility.h"

namespace utility
{
    GLuint matToTexture(const cv::Mat &mat)
    {
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // OpenCV 是 BGR 格式，转换为 RGB 格式
        cv::Mat matRGB;
        cv::cvtColor(mat, matRGB, cv::COLOR_BGR2RGB);

        // 上传纹理数据到 GPU
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, matRGB.cols, matRGB.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, matRGB.data);

        // 设置纹理参数
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        return textureID;
    }

    void SelectFolderButton(std::string &filepath, const char *button, const char *dialogKey, const char *title)
    {
        if (ImGui::Button(button))
        {
            ImGuiFileDialog::Instance()->OpenDialog(dialogKey, title, nullptr);
        }

        // 如果选择对话框打开了，处理文件夹选择
        if (ImGuiFileDialog::Instance()->Display(dialogKey))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                filepath = ImGuiFileDialog::Instance()->GetCurrentPath();
            }
            ImGuiFileDialog::Instance()->Close();
        }
    }

    bool concat(const cv::Mat &left, const cv::Mat &right, cv::Mat &dst)
    {
        cv::Mat rightc;
        if (left.size() != right.size())
            cv::resize(right, rightc, left.size());
        try
        {
            cv::hconcat(left, right, dst);
        }
        catch (const std::exception &e)
        {
            std::cerr << "concat images error : " << e.what() << '\n';
            return false;
        }
        return true;
    }
}