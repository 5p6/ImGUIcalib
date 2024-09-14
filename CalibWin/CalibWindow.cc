#include "CalibWindow.h"
#include <opencv2/opencv.hpp>
#include <filesystem>
#include <fstream>
#include "utility.h"

class CalibAlgorithmimpl;

// 这个函数封装了 ImGui::InputText，并支持 std::string
bool InputText(const char *label, std::string &str, ImGuiInputTextFlags flags = 0)
{
    // 创建一个缓冲区，确保能够容纳 string 的内容
    char buffer[64];
    std::strncpy(buffer, str.c_str(), sizeof(buffer));
    buffer[sizeof(buffer) - 1] = '\0'; // 确保缓冲区以 null 结尾

    // 使用 ImGui::InputText
    if (ImGui::InputText(label, buffer, sizeof(buffer), flags))
    {
        str = std::string(buffer); // 如果输入发生变化，更新 std::string
        return true;
    }
    return false;
}
const std::shared_ptr<CalibAlgorithm> CalibWindow::getalgo() const
{
    return algo;
}
CalibWindow::CalibWindow(
    const int &_winrow,
    const int &_wincol,
    const int &_x,
    const int &_y) : row(_winrow), col(_wincol), x(_x), y(_y)
{
    left_file.reserve(100);
    right_file.reserve(100);
    algo = CalibAlgorithm::create();
}
void CalibWindow::setResultwindow(const std::shared_ptr<ResultWindow> &reswin)
{
    reswin_ = reswin;
}
void CalibWindow::run()
{
    // 设置窗口大小和位置
    ImGui::SetNextWindowPos(ImVec2(y, x));
    ImGui::SetNextWindowSize(ImVec2(col, row));
    // 开始
    ImGui::Begin("Calib Window");
    // 选择文件夹
    InputText("", left_file);
    ImGui::SameLine();
    SelectFolderButton(left_file, "Select Folder Left", "ChooseFolderDlgKeyLeft", "Left Title");
    algo->setLeftRoot(left_file);

    if (this->algo->getNumType() == CamNumType::Stereo)
    {
        InputText("", right_file);
        ImGui::SameLine();
        SelectFolderButton(right_file, "Select Folder Right", "ChooseFolderDlgKeyRight", "Right Title");
        algo->setRiightRoot(right_file);
    }

    ImGui::InputInt("board row", &boardrow);
    ImGui::InputInt("board col", &boardcol);
    ImGui::InputDouble("board size", &boardsize);
    this->algo->setBoardRow(boardrow);
    this->algo->setBoardCol(boardcol);
    this->algo->setSquareSize(boardsize);
    if (this->algo->getBoardType() == BoardType::Square)
    {
        ImGui::InputInt("radius row", &radiusrow);
        ImGui::InputInt("radius col", &radiuscol);
        this->algo->setRadiusRow(radiusrow);
        this->algo->setRadiusCol(radiuscol);
    }

    ImGui::Text("Board Type :");
    ImGui::SameLine();
    // 创建 两个 button
    if (ImGui::RadioButton("Square", this->algo->getBoardType() == BoardType::Square))
        this->algo->setBoardType(BoardType::Square);

    ImGui::SameLine();
    if (ImGui::RadioButton("Circle", this->algo->getBoardType() == BoardType::Circle))
        this->algo->setBoardType(BoardType::Circle);
    // 创建2个 RadioButton，用来控制0, 1, 2三个flag
    ImGui::Text("Camera Num Type :");
    ImGui::SameLine();
    if (ImGui::RadioButton("Moncular", this->algo->getNumType() == CamNumType::Monocular))
        this->algo->setNumType(CamNumType::Monocular);
    ImGui::SameLine();
    if (ImGui::RadioButton("Stereo", this->algo->getNumType() == CamNumType::Stereo))
        this->algo->setNumType(CamNumType::Stereo);

    // 创建2个 RadioButton，用来控制0, 1, 2三个flag
    ImGui::Text("Camera Sensor Type :");
    ImGui::SameLine();
    // 创建 两个 button
    if (ImGui::RadioButton("Pinhole", this->algo->getSensorType() == CamSenorType::Pinhole))
        this->algo->setSensorType(CamSenorType::Pinhole);

    ImGui::SameLine();
    if (ImGui::RadioButton("Fisheye", this->algo->getSensorType() == CamSenorType::Fisheye))
        this->algo->setSensorType(CamSenorType::Fisheye);

    // omnidir 类型相机不支持
    // ImGui::SameLine();
    // if (ImGui::RadioButton("Omnidir", sensortype == CamSenorType::Omnidir))
    // {
    //     sensortype = CamSenorType::Omnidir; // 如果选中这个按钮，设置为flag 1
    // }
    if (ImGui::Button("calib"))
        calib();
    ImGui::SameLine();
    if (ImGui::Button("write"))
        write();
    ImGui::SameLine();
    this->reswin_->run();
    this->reswin_->rendererrorbars();
    this->reswin_->renderCalibimgs();
    ImGui::End();

    if (ImGui::Begin("Summary Window"))
    {
        ImGui::Text(this->algo->summary().c_str());
        ImGui::End();
    }
}

void CalibWindow::render()
{
}
void CalibWindow::calib()
{
    // std::printf("check the data \n");;
    if (this->algo->getrvecs().size() != 0)
    {
        // std::printf("clear buffer\n");
        this->algo->clear();
        this->reswin_->clear();
    }
    // std::printf("calib\n");
    if (!this->algo->calib())
        return;
    // std::printf("data\n");
    TransportData();
    // std::printf("data end\n");
}
void CalibWindow::TransportData()
{
    // std::printf("pose \n");
    // // 棋盘格
    auto rvecs = algo->getrvecs();
    auto tvecs = algo->gettvecs();
    for (int i = 0; i < rvecs.size(); i++)
    {
        glm::vec3 t(
            tvecs[i].at<double>(0, 0) * 0.05,
            tvecs[i].at<double>(1, 0) * 0.05,
            tvecs[i].at<double>(2, 0) * 0.05);
        glm::vec3 r(rvecs[i].at<double>(0, 0) * 180 / 3.1415926,
                    rvecs[i].at<double>(1, 0) * 180 / 3.1415926,
                    rvecs[i].at<double>(2, 0) * 180 / 3.1415926);
        this->reswin_->addBoard(this->algo->getBoardRow(),
                                this->algo->getBoardCol(),
                                this->algo->getSquareSize() * 0.05,
                                t,
                                r);
    }
    // std::printf("cam \n");
    // 相机
    // orginal left camera
    reswin_->addCamera(glm::vec3(0.f), glm::vec3(0.f));
    // if stereo , add right cam
    if (algo->getNumType() == CalibWindow::CamNumType::Stereo)
    {
        cv::Mat rvec;
        cv::Rodrigues(algo->getRotation(), rvec);
        cv::Mat t_ = algo->getTranslation() * 0.05;
        glm::vec3 t(
            t_.at<double>(0, 0),
            t_.at<double>(1, 0),
            t_.at<double>(2, 0));
        glm::vec3 r(rvec.at<double>(0, 0) * 180 / 3.1415926,
                    rvec.at<double>(1, 0) * 180 / 3.1415926,
                    rvec.at<double>(2, 0) * 180 / 3.1415926);
        this->reswin_->addCamera(t, r);
    }
    // std::printf("img \n");
    // // 标定图像
    this->reswin_->addcalibimg(this->algo->getCalibimgs());
    // std::printf("error \n");
    // // 重投影误差
    std::vector<double> le = this->algo->getLeftReproerr();
    this->reswin_->addlefterrors(le);
    if (this->algo->getNumType() == CalibWindow::CamNumType::Stereo)
    {
        std::vector<double> re = this->algo->getRightReproerr();
        this->reswin_->addrighterrors(re);
    }
}
void CalibWindow::write() const
{
    this->algo->write();
}
void CalibWindow::SelectFolderButton(std::string &filepath, const char *button, const char *dialogKey, const char *title)
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
void CalibWindow::ShowFileDialog(std::string &filepath, const char *button, const char *dialogKey, const char *title)
{
    // 当按下"选择文件夹"按钮时，打开选择文件夹对话框
    if (ImGui::Button(button))
    {
        ImGuiFileDialog::Instance()->OpenDialog(dialogKey, title, nullptr);
    }
    std::string selectedFolder;
    char fileNameBuffer[256] = "";
    // 如果文件夹选择对话框打开
    if (ImGuiFileDialog::Instance()->Display(dialogKey))
    {
        // 如果用户点击了 "OK"
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            // 获取选择的文件夹路径
            selectedFolder = ImGuiFileDialog::Instance()->GetCurrentPath();
            std::cout << "Selected Folder: " << selectedFolder << std::endl;
        }

        // 关闭文件夹选择对话框
        ImGuiFileDialog::Instance()->Close();
    }

    // 如果用户已经选择了文件夹
    if (!selectedFolder.empty())
    {
        // 显示输入文件名的文本框
        ImGui::InputText("", fileNameBuffer, 256);

        // 按下"创建文件"按钮时执行文件创建操作
        if (ImGui::Button("Create File"))
        {
            // 检查文件名是否为空
            if (strlen(fileNameBuffer) > 0)
            {
                // 组合文件夹路径和文件名
                std::string filePath = selectedFolder + "/" + fileNameBuffer;

                // 创建文件
                std::ofstream outFile(filePath);
                if (outFile)
                {
                    std::cout << "File created: " << filePath << std::endl;
                    outFile.close();
                }
                else
                {
                    std::cerr << "Error creating file: " << filePath << std::endl;
                }
            }
            else
            {
                std::cerr << "File name is empty!" << std::endl;
            }
        }
    }
}
class DLLEXPORT CalibAlgorithmimpl : public CalibAlgorithm
{
public:
    CalibAlgorithmimpl(const std::string &_param_path)
    {
        read(_param_path);
    }
    CalibAlgorithmimpl()
    {
        // default parameters
        this->numtype = CalibWindow::CamNumType::Monocular;
        this->sensortype = CalibWindow::CamSenorType::Pinhole;
        this->boardtype = CalibWindow::BoardType::Square;
        this->radius_size = cv::Size(5, 5);
    }
    virtual void setBoardRow(const int &row)
    {
        board_size.height = row;
    }
    virtual void setBoardCol(const int &col)
    {
        board_size.width = col;
    }
    virtual void setRadiusRow(const int &row)
    {
        radius_size.height = row;
    }

    virtual void setRadiusCol(const int &col)
    {
        radius_size.width = col;
    }

    virtual void setSquareSize(const double &size)
    {
        this->size = size;
    }
    void setNumType(CalibWindow::CamNumType _numtype)
    {
        numtype = _numtype;
    }
    void setSensorType(CalibWindow::CamSenorType _sensortype)
    {
        sensortype = _sensortype;
    }
    void setLeftRoot(const std::string &_left_root)
    {
        left_root = _left_root;
    }
    void setRiightRoot(const std::string &_right_root)
    {
        right_root = _right_root;
    }

    virtual void setBoardType(const CalibWindow::BoardType board_type)
    {
        this->boardtype = board_type;
    }
    virtual CalibWindow::CamNumType getNumType() const
    {
        return numtype;
    }

    virtual CalibWindow::CamSenorType getSensorType() const
    {
        return sensortype;
    }
    virtual int getBoardRow() const
    {
        return board_size.height;
    }

    virtual int getBoardCol() const
    {
        return board_size.width;
    }
    virtual double getSquareSize() const
    {
        return this->size;
    }
    virtual CalibWindow::BoardType getBoardType() const
    {
        return this->boardtype;
    }
    virtual int getRadiusRow() const
    {
        return radius_size.height;
    }

    virtual int getRadiusCol() const
    {
        return radius_size.width;
    }

    virtual std::vector<cv::Mat> getrvecs() const
    {
        return rvecs_l;
    }

    virtual std::vector<cv::Mat> gettvecs() const
    {
        return tvecs_l;
    }

    virtual cv::Mat getRotation() const
    {
        return R;
    }

    virtual cv::Mat getTranslation() const
    {
        return t;
    }
    virtual std::vector<double> getLeftReproerr() const
    {
        return left_reproerr;
    }

    virtual std::vector<double> getRightReproerr() const
    {
        return right_reproerror;
    }
    virtual std::vector<cv::Mat> getCalibimgs() const
    {
        return calib_imgs;
    }
    // 总结
    virtual std::string summary() const
    {
        return message.str();
    }

public:
    // 标定程序
    bool calib();

    // 写入程序
    bool write() const;

    // 读取已有的参数
    bool read(const std::string &param_path);

    // 重新标定时，需要清除之前的数据
    bool clear();

private:
    // 标定图像获取
    bool getfiles();

    // 角点查找

    bool getcalibcorner();

    // 标定算法
    bool calibalgo();

    // 重投影误差计算
    void calcerror();

    // 获得文件夹下的图像路径
    std::vector<std::string> getFilesFromPath(const std::string &path);

    // 计算重投影误差
    double computeReprojectionError(const std::vector<cv::Point3f> &objectPoints,
                                    const std::vector<cv::Point2f> &imagePoints,
                                    const cv::Mat &cameraMatrix,
                                    const cv::Mat &distCoeffs,
                                    const cv::Mat &rvec,
                                    const cv::Mat &tvec);
    void record();

private:
    // 相机、标定板信息
    double size;
    cv::Size board_size, radius_size;
    CalibWindow::CamNumType numtype;
    CalibWindow::CamSenorType sensortype;
    CalibWindow::BoardType boardtype;
    std::stringstream message;

private:
    // 左右目根目录
    std::string left_root, right_root;
    // 标定图像
    std::vector<cv::Mat> calib_imgs;
    // 中间生成值
    std::vector<std::string> left_paths, right_paths;
    // 旋转向量、平移向量，外参
    std::vector<cv::Mat> rvecs_l, tvecs_l;
    std::vector<cv::Mat> rvecs_r, tvecs_r;
    // 中间点
    std::vector<std::vector<cv::Point2f>> left_points;
    std::vector<std::vector<cv::Point2f>> right_points;
    std::vector<std::vector<cv::Point3f>> obj_points;
    // 误差
    std::vector<double> left_reproerr, right_reproerror;
    // 图像尺寸
    cv::Size img_size;

private:
    // 内参
    cv::Mat K_l, K_r, D_l, D_r;
    cv::Mat K_ls, K_rs, D_ls, D_rs;
    // 双目外参
    cv::Mat R, t;
};
bool CalibAlgorithmimpl::calib()
{
    // std::printf("record\n");
    record();
    // std::printf("get files\n");
    if (!getfiles())
        return false;
    // std::printf("get corners\n");
    if (!getcalibcorner())
        return false;
    // std::printf("get calib\n");
    if (!calibalgo())
        return false;
    // std::printf("get errors\n");
    calcerror();
    return true;
}

bool CalibAlgorithmimpl::clear()
{
    // 路径
    left_paths.clear();
    right_paths.clear();

    // 点对
    left_points.clear();
    right_points.clear();
    obj_points.clear();

    // 误差
    left_reproerr.clear();
    right_reproerror.clear();

    // 标定图像
    calib_imgs.clear();
    return true;
}
std::vector<std::string> CalibAlgorithmimpl::getFilesFromPath(const std::string &path)
{
    std::vector<std::string> paths;
    paths.reserve(15);
    for (const auto &entry : std::filesystem::directory_iterator(path))
    {
        if (std::filesystem::is_regular_file(entry))
        {
            paths.push_back(entry.path().string());
        }
    }
    return paths;
}
bool CalibAlgorithmimpl::getfiles()
{
    message << "begin to get the calibration image paths" << std::endl;
    if (left_root.empty())
        return false;
    if (numtype == CalibWindow::CamNumType::Stereo && right_root.empty())
        return false;
    left_paths = getFilesFromPath(left_root);
    left_points.reserve(left_paths.size());
    left_reproerr.reserve(left_paths.size());
    // 右目
    if (numtype == CalibWindow::CamNumType::Stereo && (!right_root.empty()))
    {
        right_paths = getFilesFromPath(right_root);
        right_points.reserve(right_paths.size());
        right_reproerror.reserve(right_paths.size());
    }
    obj_points.reserve(left_paths.size());
    calib_imgs.reserve(left_paths.size());
    return true;
}

// 角点查找
bool CalibAlgorithmimpl::getcalibcorner()
{
    message << "get corners" << std::endl;
    std::vector<cv::Point2f> left_corners;
    std::vector<cv::Point2f> right_corners;
    // 图像
    cv::Mat left_img, right_img;
    cv::Mat left_gray, right_gray;
    // 左标志
    bool left_ret, right_ret;
    // std::printf("corner detect\n");
    // 右标志
    for (int i = 0; i < left_paths.size(); i++)
    {
        // std::printf("corner detect index : %d \n",i);
        // std::printf("left paths : %s \n",left_paths[i].c_str());
        left_img = cv::imread(left_paths[i]);
        cv::cvtColor(left_img, left_gray, cv::COLOR_BGR2GRAY);
        if (numtype == CalibWindow::CamNumType::Stereo)
        {
            // std::printf("right paths : %s \n",right_paths[i].c_str());
            right_img = cv::imread(right_paths[i]);
            cv::cvtColor(right_img, right_gray, cv::COLOR_BGR2GRAY);
        }
        // std::printf("find corners \n");
        // 角点查找
        if (boardtype == CalibWindow::BoardType::Square)
        {
            left_ret = cv::findChessboardCorners(left_gray, board_size, left_corners, cv::CALIB_CB_ADAPTIVE_THRESH);
            if (numtype == CalibWindow::CamNumType::Stereo)
                right_ret = cv::findChessboardCorners(right_gray, board_size, right_corners, cv::CALIB_CB_ADAPTIVE_THRESH);
        }
        else
        {
            left_ret = cv::findCirclesGrid(left_gray, board_size, left_corners, cv::CALIB_CB_ADAPTIVE_THRESH);
            if (numtype == CalibWindow::CamNumType::Stereo)
                right_ret = cv::findCirclesGrid(right_gray, board_size, right_corners, cv::CALIB_CB_ADAPTIVE_THRESH);
        }
        // std::printf("continue check\n");
        if (numtype == CalibWindow::CamNumType::Monocular && !left_ret)
        {
            message << "find corners fault,the path of image : " << left_paths[i] << std::endl;
            continue;
        }
        else if (numtype == CalibWindow::CamNumType::Stereo && !(left_ret & right_ret))
        {
            message << "find corners fault,the path of image : " << left_paths[i] << " && " << right_paths[i] << std::endl;
            continue;
        }

        if (numtype == CalibWindow::CamNumType::Monocular)
            message << "find corners successfully,the path of image : " << left_paths[i] << std::endl;
        else
            message << "find corners successfully,the path of image : " << left_paths[i] << " && " << right_paths[i] << std::endl;
        // 亚角点查找
        if (boardtype == CalibWindow::BoardType::Square)
        {
            // 亚角点
            cv::cornerSubPix(left_gray, left_corners, radius_size, cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::Type::EPS + cv::TermCriteria::Type::MAX_ITER, 100, 1e-6));
            if (numtype == CalibWindow::CamNumType::Stereo)
                cv::cornerSubPix(right_gray, right_corners, radius_size, cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::Type::EPS + cv::TermCriteria::Type::MAX_ITER, 100, 1e-6));
        }
        // std::printf("draw corners\n");
        cv::drawChessboardCorners(left_img, board_size, left_corners, left_ret);
        // std::printf("draw left corners\n");
        if (numtype == CalibWindow::CamNumType::Stereo)
        {
            // std::printf("draw right corners\n");
            cv::drawChessboardCorners(right_img, board_size, right_corners, right_ret);
            // 双目
            cv::Mat dst;
            utility::concat(left_img, right_img, dst);
            if(dst.rows > 1024 || dst.cols > 1024)
            cv::resize(dst,dst,cv::Size(),0.3,0.3);
            // std::printf("push corners end \n");
            calib_imgs.push_back(dst);
            // std::printf("push corners end \n");
        }
        else
        {
            // 单目
            calib_imgs.push_back(left_img);
        }
        // cv::imshow("left", left_img);
        // if (numtype == CalibWindow::CamNumType::Stereo)
        //     cv::imshow("right", right_img);
        // cv::waitKey();
        // 角点推入
        // std::printf("push corner points\n");
        left_points.push_back(left_corners);
        if (numtype == CalibWindow::CamNumType::Stereo)
            right_points.push_back(right_corners);
    }
    // std::printf("corner detect end \n");
    cv::destroyAllWindows();
    // 如果没有检测到角点，返回
    if (left_points.empty())
        return false;
    // 尺寸
    img_size = left_img.size();
    // 世界点
    int board_n = this->board_size.area();
    int cols = this->board_size.width;
    std::vector<cv::Point3f> points;
    points.reserve(board_n + 1);
    for (auto corners : left_points)
    {
        points.clear();
        for (int i = 0; i < board_n; i++)
        {
            float x = static_cast<float>(i % cols);
            cv::Point3f point(x, (float)(i - x) / cols, 0);
            points.push_back(point);
        }
        obj_points.push_back(points);
    }
    return true;
}

bool CalibAlgorithmimpl::calibalgo()
{
    message << "calibration algorithm" << std::endl;
    cv::Mat R_l, R_r, P_l, P_r, Q;
    double rms1, rms2;
    double rms;
    switch (sensortype)
    {
    case CalibWindow::CamSenorType::Pinhole:
    {
        message << "begin to calib" << std::endl;
        cv::Mat E, F;
        // 各自标定
        try
        {
            rms1 = cv::calibrateCamera(obj_points, left_points, img_size, K_l, D_l, rvecs_l, tvecs_l, 0, cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 60, 0));
        }
        catch (const std::exception &e)
        {
            message << "Monocular Pinhole Left exception : " << e.what() << '\n';
            return false;
        }
        message << "left cam residual  :" << rms1 << std::endl;
        message << "left cam K_l : \n"
                << K_l << "\n"
                << "D_l : \n"
                << D_l << std::endl;
        K_ls = K_l.clone();
        D_ls = D_l.clone();
        // 本征矩阵和基本矩阵
        // 双目检测
        if (numtype != CalibWindow::CamNumType::Stereo)
            return true;
        try
        {
            rms2 = cv::calibrateCamera(obj_points, right_points, img_size, K_r, D_r, rvecs_r, tvecs_r, 0, cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 100, 1e-8));
        }
        catch (const std::exception &e)
        {
            message << "Monocular Pinhole Right exception : " << e.what() << '\n';
            return false;
        }
        message << "right cam residual : " << rms2 << std::endl;
        message << "right cam K_r : \n"
                << K_r << "\n"
                << "D_r : \n"
                << D_r << std::endl;
        K_rs = K_r.clone();
        D_rs = D_r.clone();
        try
        {
            // RTEE
            rms = cv::stereoCalibrate(
                obj_points,
                left_points,
                right_points,
                K_l,
                D_l,
                K_r,
                D_r,
                img_size,
                R,
                t,
                E,
                F,
                0,
                cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 60, 1e-8));
        }
        catch (const std::exception &e)
        {
            message << "Stereo Pinhole exception : " << e.what() << '\n';
            return false;
        }
        message << "stereo res :" << rms << std::endl;
        message << "left  K_l :\n"
                << K_l << "\n"
                << "left D_l : \n"
                << D_l << std::endl;
        message << "right K_r :\n"
                << K_r << "\n"
                << "right D_r : \n"
                << D_r << std::endl;
        message << "R : \n"
                << R << std::endl;
        message << "t : \n "
                << t << std::endl;
        break;
    }
    case CalibWindow::CamSenorType::Fisheye:
    {

        try
        {
            rms1 = cv::fisheye::calibrate(obj_points, left_points, img_size, K_l, D_l, rvecs_l, tvecs_l, 0, cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 100, 1e-5));
        }
        catch (const std::exception &e)
        {
            message << "Monocular Fisheye Left exception : " << e.what() << '\n';
            return false;
        }

        message << "left cam residual  :" << rms1 << std::endl;
        message << "left cam K_l : \n"
                << K_l << "\n"
                << "D_l : \n"
                << D_l << std::endl;
        K_ls = K_l.clone();
        D_ls = D_l.clone();

        // 双目检测
        if (numtype != CalibWindow::CamNumType::Stereo)
            return true;
        try
        {
            rms2 = cv::fisheye::calibrate(obj_points, right_points, img_size, K_r, D_r, rvecs_r, tvecs_r, 0, cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 100, 1e-5));
        }
        catch (const std::exception &e)
        {
            message << "Monocular Fisheye Right exception : " << e.what() << '\n';
            return false;
        }
        K_rs = K_r.clone();
        D_rs = D_r.clone();
        message << "right cam residual : " << rms2 << std::endl;
        message << "right cam K_r : \n"
                << K_r << "\n"
                << "D_r : \n"
                << D_r << std::endl;
        // 联合标定
        message << "begin to stereo calib" << std::endl;
        try
        {
            // R,T
            double rms = cv::fisheye::stereoCalibrate(
                obj_points,
                left_points,
                right_points,
                K_l,
                D_l,
                K_r,
                D_r,
                img_size,
                R,
                t);
        }
        catch (const std::exception &e)
        {
            message << "Stereo Fisheye exception : " << e.what() << '\n';
            return false;
        }
        message << "stereo res :" << rms << std::endl;
        message << "left  K_l :\n"
                << K_l << "\n"
                << "left D_l : \n"
                << D_l << std::endl;
        message << "right K_r :\n"
                << K_r << "\n"
                << "right D_r : \n"
                << D_r << std::endl;
        message << "R : \n"
                << R << std::endl;
        message << "t : \n "
                << t << std::endl;
        break;
    }
    default:
        message << "sensor type error ,please check your camera sensor type" << std::endl;
        return false;
        break;
    }
    return true;
}
// 重投影误差计算
void CalibAlgorithmimpl::calcerror()
{
    for (int i = 0; i < left_points.size(); i++)
        left_reproerr.push_back(computeReprojectionError(
            obj_points[i], left_points[i], K_ls, D_ls, rvecs_l[i], tvecs_l[i]));
    if (numtype == CalibWindow::CamNumType::Stereo)
        for (int i = 0; i < right_points.size(); i++)
            right_reproerror.push_back(computeReprojectionError(
                obj_points[i], right_points[i], K_rs, D_rs, rvecs_r[i], tvecs_r[i]));
}
bool CalibAlgorithmimpl::read(const std::string &param_path)
{
    cv::FileStorage file(param_path, cv::FileStorage::READ);
    if (!file.isOpened())
    {
        message << "the parameters file does not open !" << std::endl;
        return false;
    }
    // 类型一查看
    sensortype = CalibWindow::CamSenorType::Pinhole;
    if (file["Camera_SensorType"].string() == "Fisheye")
        sensortype = CalibWindow::CamSenorType::Fisheye;
    // 参数导入
    file["K_l"] >> K_l;
    file["D_l"] >> D_l;
    // 类型二查看
    numtype = CalibWindow::CamNumType::Monocular;
    img_size = cv::Size(file["width"], file["height"]);
    if (file["Camera_NumType"].string() == "Stereo")
    {
        numtype = CalibWindow::CamNumType::Stereo;
        file["K_r"] >> K_r;
        file["D_r"] >> D_r;
        file["R"] >> R;
        file["t"] >> t;
    }
    return true;
}

bool CalibAlgorithmimpl::write() const
{
    cv::FileStorage file("./calib_parameters.yaml", cv::FileStorage::WRITE);
    if (!file.isOpened())
        return false;
    // 查看是否标定了？
    if (K_l.empty())
    {
        std::cout << "there is not param here,please call the calib program !" << std::endl;
        return false;
    }

    if (sensortype == CalibWindow::CamSenorType::Pinhole)
    {
        file << "Camera_SensorType"
             << "Pinhole";
    }
    else
    {
        file << "Camera_SensorType"
             << "Fisheye";
    }

    switch (numtype)
    {
    case CalibWindow::CamNumType::Monocular:
    {
        file << "Camera_NumType"
             << "Monocular"
             << "K_l" << K_l
             << "D_l" << D_l;
        break;
    }
    case CalibWindow::CamNumType::Stereo:
    {
        file << "Camera_NumType"
             << "Stereo"
             << "K_l" << K_l
             << "D_l" << D_l
             << "K_r" << K_r
             << "D_r" << D_r
             << "R" << R
             << "t" << t;
        break;
    }
    default:
        return false;
    }
    file << "height" << img_size.height
         << "width" << img_size.width;
    file.release();
    return true;
}

void CalibAlgorithmimpl::record()
{
    message.str(std::string());
    std::string numstr = numtype == CalibWindow::CamNumType::Monocular ? "Monocular" : "Stereo";
    std::string senstr = sensortype == CalibWindow::CamSenorType::Pinhole ? "Pinhole" : "Fisheye";
    message << "---------------------------------------------------------------------------" << std::endl;
    message << "camera number type : " << numstr << "\n"
            << "camera sensor type : " << senstr << "\n"
            << "board corners size : " << board_size << "\n"
            << "square size : " << size << "\n"
            << "left root : " << left_root << std::endl;
    if (!right_root.empty())
    {
        message << "right root : " << right_root
                << std::endl;
    }
}
// void
double CalibAlgorithmimpl::computeReprojectionError(const std::vector<cv::Point3f> &objectPoints,
                                                    const std::vector<cv::Point2f> &imagePoints,
                                                    const cv::Mat &cameraMatrix,
                                                    const cv::Mat &distCoeffs,
                                                    const cv::Mat &rvec,
                                                    const cv::Mat &tvec)
{
    std::vector<cv::Point2f> projectedPoints;

    // 将世界坐标系中的 3D 点投影到图像平面上
    if (sensortype == CalibWindow::CamSenorType::Pinhole)
        cv::projectPoints(objectPoints, rvec, tvec, cameraMatrix, distCoeffs, projectedPoints);
    else if (sensortype == CalibWindow::CamSenorType::Fisheye)
        cv::fisheye::projectPoints(objectPoints, rvec, tvec, cameraMatrix, distCoeffs, projectedPoints);
    double totalError = 0.0;
    int nPoints = objectPoints.size();
    // 计算重投影误差 (使用欧几里得距离)
    for (int i = 0; i < nPoints; ++i)
    {
        double error = cv::norm(imagePoints[i] - projectedPoints[i]);
        totalError += error;
    }

    // 返回平均重投影误差
    return totalError / nPoints;
}

std::shared_ptr<CalibAlgorithm> CalibAlgorithm::create(const std::string &_param_path)
{
    return std::make_shared<CalibAlgorithmimpl>(_param_path);
}
std::shared_ptr<CalibAlgorithm> CalibAlgorithm::create()
{
    return std::make_shared<CalibAlgorithmimpl>();
}