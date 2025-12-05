#include "history.h"
#include "ui_history.h"

/**
功能：历史版本信息窗口
修改：sjm
参数：QWidget* parent-创建编辑器控件的父窗口，QMainWindow(parent)-调用基类QMainWindow的构造函数，ui(new Ui::history)-动态创建Ui::history界面类实例
返回值：-
**/
history::history(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::history)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("时钟调试V5.0"));
    this->setWindowIcon(QIcon(":/jpg/picture/3.jpg"));
}

/**
功能：析构函数释放内存和空间
修改：sjm
参数：-
返回值：-
**/
history::~history()
{
    delete ui;
}

/**
功能：按键关闭窗口槽函数
修改：sjm
参数：-
返回值：-
**/
void history::on_pushButton_clicked()
{
    this->close();
}
