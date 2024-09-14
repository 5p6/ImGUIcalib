#include "MainWindow.h"

#include <iostream>
#include <vector>


int main()
{
    std::unique_ptr<MainWindow> win = std::make_unique<MainWindow>(540,1080);
    return 0;
}