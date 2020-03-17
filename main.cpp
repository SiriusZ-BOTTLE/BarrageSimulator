#include "MainWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    //根据当前时间设置随机种子
    qsrand(static_cast<unsigned>(QTime(0,0,0).secsTo(QTime::currentTime())));

    return a.exec();
}
