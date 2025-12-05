#include "mainwindow.h"
#include <QApplication>
#include <QStyleFactory>

/**
功能：主函数入口
修改：sjm
参数：argc-输入参数，*argv[]-输入的数组参数
返回值：a.exec()-创建MainWindow窗口
**/
int main(int argc, char *argv[])
{

#if (QT_VERSION >= QT_VERSION_CHECK(5,9,0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
        {
            QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
            QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, true);
            QApplication::setHighDpiScaleFactorRoundingPolicy(
            Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
        }

    QApplication a(argc, argv);
    QApplication::setStyle(QStyleFactory::create("Windows"));
    MainWindow w;
    w.show();
    return a.exec();
}
