#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPixmap>

/**
功能：主窗口函数--设置主界面，主窗口信息的显示
修改：sjm
参数：QWidget* parent-创建编辑器控件的父窗口，QMainWindow(parent)-调用基类QMainWindow的构造函数，ui(new Ui::MainWindow)-动态创建Ui::MainWindow界面类实例
返回值：-
**/
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QFont font;
    font.setBold(true);
    font.setPointSize(16); // 设置字体大小为12
    font.setFamily("微软雅黑");

    QFont fontv;
    fontv.setBold(true);
    fontv.setPointSize(25);

    this->setWindowIcon(QIcon(":/jpg/picture/3.jpg"));
    this->setWindowTitle(tr("WAVE-时钟调试V5.5"));
    // 加载图片文件
    QImage image(":/jpg/picture/open.png");
    QImage scaledImage = image.scaled(this->size(), Qt::KeepAspectRatio);

    QLabel* label = new QLabel(this);
    label->setPixmap(QPixmap::fromImage(scaledImage));
    label->setScaledContents(true);
    label->setGeometry(0, 0, this->width(), this->height());

    QLabel* versionLabel = new QLabel(this);
    versionLabel->setGeometry(800, 35, 250, 30);

    QPushButton* button = new QPushButton(this);
    button->setGeometry(450, 560, 130, 40);

    // 加载图标文件
    QIcon icon(":/jpg/picture/enter.png"); // 替换为实际的图标文件路径

    // 设置按钮图标
    button->setIcon(icon);
    button->setIconSize(QSize(130, 50)); // 设置图标大小
    // 设置版本信息
    versionLabel->setText("Version5.5");
    versionLabel->setFont(fontv);
    versionLabel->setStyleSheet("color: white;");
    button->setFont(font);
    // 设置按钮样式表
    button->setStyleSheet("QPushButton { background-color: white; border-radius: 20px; }");
    // 处理按钮点击事件的代码
    connect(button, &QPushButton::clicked, this, &MainWindow::enter);


}

/**
功能：析构函数释放内存和空间
修改：sjm
参数：-
返回值：-
**/
MainWindow::~MainWindow()
{
    delete ui;
}

/**
功能：按键切换窗口槽函数
修改：sjm
参数：-
返回值：-
**/
void MainWindow::enter()
{
    Selectui=new SelectDevice();
    Selectui->show();
    this->close();

}
