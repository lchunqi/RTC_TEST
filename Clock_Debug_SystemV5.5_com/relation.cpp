#include "relation.h"
#include "ui_relation.h"

/**
功能：联系我们窗口
修改：sjm
参数：QWidget* parent-创建编辑器控件的父窗口，QMainWindow(parent)-调用基类QMainWindow的构造函数，ui(new Ui::relation)-动态创建Ui::relation界面类实例
返回值：-
**/
relation::relation(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::relation)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("联系我们"));
    this->setWindowIcon(QIcon(":/jpg/picture/3.jpg"));
    // 连接链接点击事件的槽函数
    connect(ui->label3, &QLabel::linkActivated, this, [](const QString& link){
        QDesktopServices::openUrl(QUrl(link));
    });

}

/**
功能：析构函数释放内存和空间
修改：sjm
参数：-
返回值：-
**/
relation::~relation()
{
    delete ui;
}

/**
功能：按键关闭窗口槽函数
修改：sjm
参数：-
返回值：-
**/
void relation::on_pushButton_clicked()
{
    this->close();
}
