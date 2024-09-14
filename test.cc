#include "MainWindow.h"

#include <iostream>
#include <vector>

int main()
{
    std::unique_ptr<MainWindow> win = std::make_unique<MainWindow>(960,1840);
    return 0;
}