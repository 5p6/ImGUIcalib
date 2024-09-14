#ifndef BASEWINDOW_H
#define BASEWINDOW_H

#ifdef _WIN32
#ifdef DLLEXPORT
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif
#else
#define DLLEXPORT
#endif

class DLLEXPORT BaseWindow
{
public:
    /** @brief 用来执行文本框、滑动条等基本交互，主要是用来获取关于render的信息
     */
    virtual void run() = 0;

    /** @brief 主窗口渲染，渲染三维场景
     */
    virtual void render() = 0;
};

#endif