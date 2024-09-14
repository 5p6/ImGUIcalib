#include "ResultWindow.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

ResultWindow::Grid::Grid(
    const int &_row,
    const int &_col,
    const double &_size,
    const glm::vec3 &_translation,
    const glm::vec3 &_rotation) : rows(_row), cols(_col), size(_size), translation(_translation), rotation(_rotation) {}
// ResultWindow
ResultWindow::Camera::Camera(
    const glm::vec3 &_translation,
    const glm::vec3 &_rotation) : translation(_translation), rotation(_rotation)
{
}
void ResultWindow::clear()
{
    grids.clear();
    cams.clear();
    errors_l.clear();
    errors_r.clear();
    xs_l.clear();
    xs_r.clear();
    textimgs.clear();
}
// 初始化
ResultWindow::ResultWindow() {}

// 添加棋盘格
void ResultWindow::addBoard(
    const int &_row,
    const int &_col,
    const double &_size,
    const glm::vec3 &_translation,
    const glm::vec3 &_rotation)
{
    grids.emplace_back(
        Grid(_row, _col, _size, _translation, _rotation));
}
void ResultWindow::addCamera(
    const glm::vec3 &_translation,
    const glm::vec3 &_rotation)
{
    cams.emplace_back(
        Camera(_translation, _rotation));
}

void ResultWindow::addlefterrors(std::vector<double> &_errors)
{
    // 重投影误差
    errors_l.swap(_errors);
    xs_l.reserve(errors_l.size());
    for (int i = 0; i < errors_l.size(); i++)
        xs_l.emplace_back(static_cast<double>(i - 0.1));
}

void ResultWindow::addrighterrors(std::vector<double> &_errors)
{
    // 左目
    errors_r.swap(_errors);
    xs_r.reserve(errors_r.size());
    for (int i = 0; i < errors_r.size(); i++)
        xs_r.emplace_back(static_cast<double>(i + 0.1));
}

void ResultWindow::run()
{
    // 点击 render 按钮开始渲染
    if (ImGui::Button("render"))
    {
        renderflag = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("stop render"))
    {
        renderflag = false;
    }
    ImGui::SameLine();
    if (ImGui::Button("clear mem"))
    {
        grids.clear();
        cams.clear();

        errors_l.clear();
        errors_r.clear();
        xs_l.clear();
        xs_r.clear();
    }
}
void ResultWindow::render()
{
    if (!renderflag)
        return;
    if (grids.size() != 0)
    {
        // 棋盘格

        for (const Grid &grid : grids)
        {
            glPushMatrix();
            rendersingle(grid);
            glPopMatrix();
        }
    }
    // 相机
    if (cams.size() != 0)
    {

        for (const Camera &cam : cams)
        {
            glPushMatrix();
            rendersingle(cam);
            glPopMatrix();
        }
    }
}

void ResultWindow::rendersingle(const Grid &grid)
{

    // 平移和旋转变换，先旋转再平移
    /*
    T = R t
        0 1
    */
    // 设置初始位姿
    glRotatef(grid.rotation.x, 1.0f, 0.0f, 0.0f);
    glRotatef(grid.rotation.y, 0.0f, 1.0f, 0.0f);
    glRotatef(grid.rotation.z, 0.0f, 0.0f, 1.0f);
    glTranslatef(grid.translation.x, grid.translation.y, grid.translation.z);
    // 开始以线条渲染棋盘格
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);

    // 渲染纵向线条
    for (int i = 0; i <= grid.cols; ++i)
    {
        float x = static_cast<float>(i) / grid.cols;
        glVertex3f(x * grid.size, 0.0f, 0.0f);
        glVertex3f(x * grid.size, 1.0f * grid.size, 0.0f);
    }

    // 渲染横向线条
    for (int i = 0; i <= grid.rows; ++i)
    {
        float y = static_cast<float>(i) / grid.rows;
        glVertex3f(0.0f, y * grid.size, 0.0f);
        glVertex3f(1.0f * grid.size, y * grid.size, 0.0f);
    }
    glEnd();
}

void ResultWindow::rendersingle(const Camera &cam)
{

    // 平移和旋转变换，先平移在旋转
    /*
    T = R t
        0 1
    */
    // 旋转
    glRotatef(cam.rotation.x, 1.0f, 0.0f, 0.0f);
    glRotatef(cam.rotation.y, 0.0f, 1.0f, 0.0f);
    glRotatef(cam.rotation.z, 0.0f, 0.0f, 1.0f);
    // 平移
    glTranslatef(cam.translation.x, cam.translation.y, cam.translation.z);
    // 开始渲染
    glBegin(GL_LINES);
    // X轴 红色
    glColor3f(1.f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.5f, 0.0f, 0.0f);
    // Y轴 绿色
    glColor3f(0.0f, 1.f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.5f, 0.0f);
    // Z轴 蓝色
    glColor3f(0.0f, 0.0f, 1.f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.5f);
    glEnd();
}

//
void ResultWindow::rendererrorbars()
{
    if (errors_l.size() == 0 || !renderflag)
        return;
    // 启动一个新的 ImGui 窗口
    if (ImGui::Begin("Reproject Error Bar"))
    {
        // 启动 ImPlot 绘图区域
        if (ImPlot::BeginPlot("Camera Error and Parameters", "Index", "Value"))
        {
            ImPlot::PlotBars("left", xs_l.data(), errors_l.data(), errors_l.size(), 0.1, 0);
            if (!errors_r.empty())
                ImPlot::PlotBars("right", xs_r.data(), errors_r.data(), errors_r.size(), 0.1, 0);
            ImPlot::EndPlot();
        }
        ImGui::End();
    }
}
// 标定图像
void ResultWindow::renderCalibimgs()
{
    if (textimgs.size() == 0 || !renderflag)
        return;
    ImGui::Begin("Multiple Calib Images Example");

    ImVec2 size(400, 400); // 图像显示的大小

    for (size_t i = 0; i < textimgs.size(); ++i)
    {
        ImGui::Text("Calib Image %d:", static_cast<int>(i + 1));
        ImGui::Image((void *)(intptr_t)textimgs[i], size);
    }
    ImGui::End();
}

void ResultWindow::addcalibimg(const std::vector<cv::Mat> &imgs)
{
    if (textimgs.capacity() < imgs.capacity())
        textimgs.reserve(imgs.size());

    for (const cv::Mat &img : imgs)
        textimgs.emplace_back(utility::matToTexture(img));
}
GLuint ResultWindow::matToTexture(const cv::Mat &mat, GLenum minFilter, GLenum magFilter, GLenum wrapFilter)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // 处理 OpenCV 中的 BGR -> RGB 转换
    GLenum inputFormat = GL_BGR_EXT;
    if (mat.channels() == 4)
        inputFormat = GL_BGRA_EXT;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mat.cols, mat.rows, 0, inputFormat, GL_UNSIGNED_BYTE, mat.ptr());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapFilter);

    glBindTexture(GL_TEXTURE_2D, 0);
    return textureID;
}