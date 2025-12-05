#include "notice.h"
#include "ui_notice.h"

/**
功能：注意事项窗口
修改：sjm
参数：QWidget* parent-创建编辑器控件的父窗口，QMainWindow(parent)-调用基类QMainWindow的构造函数，ui(new Ui::notice)-动态创建Ui::notice界面类实例
返回值：-
**/
notice::notice(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::notice)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("注意事项"));
    this->setWindowIcon(QIcon(":/jpg/picture/3.jpg"));

}

/**
功能：析构函数释放内存和空间
修改：sjm
参数：-
返回值：-
**/
notice::~notice()
{
    delete ui;
}

/**
功能：按键关闭窗口槽函数
修改：sjm
参数：-
返回值：-
**/
void notice::on_pushButton_clicked()
{
    this->close();
}
