#ifndef CALIBWINDOW_H
#define CALIBWINDOW_H
#include "imgui.h"
#include "ImGuiFileDialog.h"
#include "implot.h"
#include "BaseWindow.h"
#include <string>
#include <vector>
#include <opencv2/core.hpp>
#include <ResultWindow.h>
class CalibAlgorithm;

class DLLEXPORT CalibWindow : public BaseWindow
{

public:
    enum class Methods
    {
        Ceres = 0,
        OpenCV
    };
    enum class BoardType
    {
        Square = 0,
        Circle
    };
    enum class CamNumType
    {
        Monocular = 0,
        Stereo
    };

    enum class CamSenorType
    {
        Pinhole = 0,
        Fisheye,
        Omnidir
    };

public:
    CalibWindow(
        const int &_winrow = 180,
        const int &_wincol = 360,
        const int &_x = 0,
        const int &_y = 0);

    // 用来执行文本框、滑动条等基本交互，主要是用来获取关于render的信息
    virtual void run();

    // 主窗口渲染
    virtual void render();

    const std::shared_ptr<CalibAlgorithm> getalgo() const;

    // 设置显示结果窗口
    void setResultwindow(const std::shared_ptr<ResultWindow> &reswin);

private:
    // 选择文件夹选项
    void SelectFolderButton(std::string &filepath, const char *button, const char *dialogKey, const char *title);

    // 保存文件夹
    void ShowFileDialog(std::string &filepath, const char *button, const char *dialogKey, const char *title);

    // 标定
    void calib();

    // 保存
    void write() const;

    // 添加位姿，包含棋盘格、相机的位姿
    void TransportData();

private:
    // 窗口信息
    int row, col, x, y;
    // 棋盘格信息
    int boardrow = 6, boardcol = 8;
    int radiusrow = 5, radiuscol = 5;
    double boardsize = 20;
    // BoardType boardtype = BoardType::Square;
    std::string left_file, right_file;
    std::string write_file;

    // 相机信息
    Methods method;

    // 算法
    std::shared_ptr<CalibAlgorithm> algo;
    std::shared_ptr<ResultWindow> reswin_;
};

class DLLEXPORT CalibAlgorithm
{

public:
    static std::shared_ptr<CalibAlgorithm> create(const std::string &_param_path);
    static std::shared_ptr<CalibAlgorithm> create();

public:
    virtual void setBoardRow(const int &row) = 0;

    virtual void setBoardCol(const int &col) = 0;

    virtual void setRadiusRow(const int &row) = 0;

    virtual void setRadiusCol(const int &col) = 0;

    virtual void setSquareSize(const double &size) = 0;

    virtual void setNumType(CalibWindow::CamNumType _numtype) = 0;

    virtual void setSensorType(CalibWindow::CamSenorType _sensortype) = 0;

    virtual void setLeftRoot(const std::string &_left_root) = 0;

    virtual void setRiightRoot(const std::string &_right_root) = 0;

    virtual void setBoardType(const CalibWindow::BoardType board_type) = 0;

    virtual CalibWindow::CamNumType getNumType() const = 0;

    virtual CalibWindow::CamSenorType getSensorType() const = 0;

    virtual int getBoardRow() const = 0;

    virtual int getBoardCol() const = 0;

    virtual double getSquareSize() const = 0;

    virtual CalibWindow::BoardType getBoardType() const = 0;

    virtual int getRadiusRow() const = 0;

    virtual int getRadiusCol() const = 0;

    virtual std::vector<cv::Mat> getrvecs() const = 0;

    virtual std::vector<cv::Mat> gettvecs() const = 0;

    virtual cv::Mat getRotation() const = 0;

    virtual cv::Mat getTranslation() const = 0;

    virtual std::vector<double> getLeftReproerr() const = 0;

    virtual std::vector<double> getRightReproerr() const = 0;

    virtual std::vector<cv::Mat> getCalibimgs() const = 0;

    virtual std::string summary() const = 0;

public:
    virtual bool calib() = 0;

    // 写入程序
    virtual bool write() const = 0;

    // 读取程序
    virtual bool read(const std::string &param_path) = 0;

    // 清除缓存
    virtual bool clear() = 0;
};

#endif