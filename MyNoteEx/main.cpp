#include "mynote.h"
#include "singleapplication.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    SingleApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);
    if(!a.isRunning())
    {
        MyNote w;
        a.w = &w;       // 保存主窗口指针
        w.show();

        return a.exec();
    }
    return 0;
}
