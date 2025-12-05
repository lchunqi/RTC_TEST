#include "rtcmessage.h"
#include "ui_rtcmessage.h"
#include <QSerialPort>
#include <QSerialPortInfo>

QSerialPort *serialPort;//串口全局变量
const char* chinese_week[] = {"日", "一", "二", "三", "四", "五", "六"};
QString HexInput;
QDateTime datatime = QDateTime::currentDateTime();//获取系统时间

/**
功能：芯片信息显示界面--寄存器显示，芯片信息显示，温度，电池显示，id码显示，寄存器读写功能实现
修改：sjm
参数：QString selectedModel-传递的选项信息，QWidget* parent-创建编辑器控件的父窗口，QMainWindow(parent)-调用基类QMainWindow的构造函数，ui(new Ui::RtcMessage)-动态创建Ui::RtcMessage界面类实例
返回值：-
**/
RtcMessage::RtcMessage(QString selectedModel,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RtcMessage)
{
    ui->setupUi(this);

    ui->rtctype->setText(selectedModel);

    serialPort = new QSerialPort;

    Serialinit();//串口初始化

    tool();//RTC界面基础信息

    timerSerial=new QTimer(this);
    // 连接串口切换信号和槽函数
    connect(ui->com_connect, QOverload<const QString&>::of(&QComboBox::currentIndexChanged),this,&RtcMessage::changeSerialPort);

    // 连接定时器信号和槽函数
    connect(timerSerial, &QTimer::timeout,this,&RtcMessage::updateSerialData);

    // 设置定时器间隔为1秒
    timerSerial->setInterval(1000);

    // 启动定时器
    timerSerial->start();


    //选择型号的数据库
    if (selectedModel == "SD3068")
    {

    sd3068();

    }else if (selectedModel == "SD8563") {

    sd8563();

    }else if (selectedModel == "SD8825") {

    sd8825();

    }else if (selectedModel == "SD8804") {

    sd8804();

    }else if (selectedModel == "SD8840") {

    sd8840();

    }else if (selectedModel == "SD8902") {

    sd8902();

    }else if (selectedModel == "SD8931") {

    sd8931();

    }else if (selectedModel == "SD8810") {

    sd8810();

    }else if (selectedModel == "SD8939") {

    sd8939();

    }else if (selectedModel == "SD8800") {

    sd8800();

    }else if (selectedModel == "SD8200") {

    sd8200();

    }else if (selectedModel == "SD2010") {

    sd2010();

    }else if (selectedModel == "SD2058") {

    sd2058();

    }else if (selectedModel == "SD2059") {

    sd2059();

    }else if (selectedModel == "SD2068") {

    sd2068();

    }else if (selectedModel == "SD2069") {

    sd2069();

    }else if (selectedModel == "SD2077") {

    sd2077();

    }else if (selectedModel == "SD2078") {

    sd2078();

    }else if (selectedModel == "SD2400") {

    sd2068();

    }else if (selectedModel == "SD2401") {

    sd2068();

    }else if (selectedModel == "SD2402") {

    sd2068();

    }else if (selectedModel == "SD2403") {

    sd2068();

    }else if (selectedModel == "SD2404") {

    sd2068();

    }else if (selectedModel == "SD2405") {

    sd2068();

    }else if (selectedModel == "SD2503") {

    sd3068();

    }else if (selectedModel == "SD2505") {

    sd3068();

    }else if (selectedModel == "SD2506") {

    sd3068();

    }else if (selectedModel == "SD2507") {

    sd3068();

    }else if (selectedModel == "SD2508") {

    sd3068();

    }else if (selectedModel == "SD2520") {

    sd3068();

    }else if (selectedModel == "SD2522") {

    sd3068();

    }else if (selectedModel == "SD2523") {

    sd3068();

    }else if (selectedModel == "SD3010") {

    sd3068();

    }else if (selectedModel == "SD3025") {

    sd3068();

    }else if (selectedModel == "SD3077") {

    sd3068();

    }else if (selectedModel == "SD3078") {

    sd3068();

    }else if (selectedModel == "SD3178") {

    sd3068();

    }else if (selectedModel == "SD3031") {

    sd3068();

    }else if (selectedModel == "SD3900") {

    sd3068();

    }else if (selectedModel == "SD8564") {

    sd8563();

    }else if (selectedModel == "SD8565") {

    sd8563();

    }else if (selectedModel == "SD8568") {

    sd8563();

    }else if (selectedModel == "SD2200") {

    sd2200();

    }else if (selectedModel == "SD2201") {

    sd2200();

    }else if (selectedModel == "SD2202") {

    sd2200();

    }else if (selectedModel == "SD2203") {

    sd2200();

    }else if (selectedModel == "SD2204") {

    sd2200();

    }

}

/**
功能：析构函数释放内存和空间
修改：sjm
参数：-
返回值：-
**/
RtcMessage::~RtcMessage()
{
    delete ui;
    delete serialPort; // 释放串口对象

}

/**
功能：串口初始化
修改：sjm
参数：-
返回值：-
**/
void RtcMessage::Serialinit()
{
    //查找可用的串口
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort serial;
        serial.setPort(info);   //设置串口
        if(serial.open(QIODevice::ReadWrite))
        {
            ui->com_connect->addItem(serial.portName());//显示串口name
            serial.close();
        }
    }
    serialPort->setPortName(ui->com_connect->currentText());//设置串口名
    if(serialPort->open(QIODevice::ReadWrite))//打开串口成功
    {
        // 添加默认串口配置
        serialPort->setBaudRate(QSerialPort::Baud19200);
        serialPort->setDataBits(QSerialPort::Data8);
        serialPort->setParity(QSerialPort::NoParity);
        serialPort->setStopBits(QSerialPort::OneStop);
        serialPort->setFlowControl(QSerialPort::NoFlowControl);

    }else{
        qDebug()<<"错误，串口打开失败，串口可能被占用!";
        QMessageBox::information(this,"错误","串口打开失败，串口未连接或者被占用!");
        qDebug() << serialPort->errorString();
    }

}

/**
功能：RTC界面基础信息
修改：sjm
参数：-
返回值：-
**/
void RtcMessage::tool(void)
{
    //芯片界面信息左下方工具栏创建
    toolbar = new QToolBar("toolbar");
    this->addToolBar(Qt::LeftToolBarArea,toolbar);

    this->setWindowIcon(QIcon(":/jpg/picture/3.jpg"));
    this->setWindowTitle(tr("时钟调试V5.5"));

    Close = new QAction (tr ( "&刷新串口" ), ui->file );
    Close->setShortcuts ( QKeySequence::Bold);
    ui->file->addAction(Close);
    connect(Close, &QAction::triggered, this, &RtcMessage::ReCom);

    Close = new QAction (tr ( "&电脑时间校时" ), ui->file );
    Close->setShortcuts ( QKeySequence::Open );
    ui->file->addAction(Close);
    connect(Close, &QAction::triggered, this, &RtcMessage::Computer_time);

    Close = new QAction (tr ( "&退出" ), ui->file );
    Close->setShortcuts ( QKeySequence::New );
    ui->file->addAction(Close);
    connect(Close, &QAction::triggered, this, &RtcMessage::close);

    Device = new QAction (tr ( "&器件选择" ), ui->deveice );
    Device->setShortcuts ( QKeySequence::Undo );
    ui->deveice->addAction(Device);
    connect(Device, &QAction::triggered,this, &RtcMessage::skip);

    Help = new QAction (tr ( "&注意事项" ), ui->help );
    ui->help->addAction(Help);
    connect(Help, &QAction::triggered, this, &RtcMessage::notices);

    Help = new QAction (tr ( "&公司主页" ), ui->help );
    ui->help->addAction(Help);
    connect(Help, &QAction::triggered, this, &RtcMessage::homepages);

    Help = new QAction (tr ( "&联系我们" ), ui->help );
    ui->help->addAction(Help);
    connect(Help, &QAction::triggered, this, &RtcMessage::relations);

    Help = new QAction (tr ( "&历史版本" ), ui->help );
    ui->help->addAction(Help);
    connect(Help, &QAction::triggered, this, &RtcMessage::historys);

    toolbar->setAllowedAreas(Qt::LeftToolBarArea | Qt::RightToolBarArea);

    label = new QLabel;
    label->setText("评估板准备就绪");
    ui->statusbar->addWidget(label);

    labe2 = new QLabel;
    labe2->setText("型号");
    ui->statusbar->addWidget(labe2);

    labe4 = new QLabel;
    labe4->setText("USB设备连接状态");
    ui->statusbar->addWidget(labe4);

    QLabel *labe5 = new QLabel(datatime.toString("yyyy-MM-dd hh:mm:ss dddd"),this);//创建时间显示Lable
    ui->statusbar->addWidget(labe5);//添加进工具栏
    labe5->setText(datatime.toString("yyyy-MM-dd hh:mm:ss dddd"));//获取系统时间
    timer = new QTimer(this);//创建定时器
    connect(timer,&QTimer::timeout,this,[=](){
    QDateTime datatime = QDateTime::currentDateTime();//获取系统时间
    labe5->setText(datatime.toString("yyyy-MM-dd hh:mm:ss dddd"));
    });
    timer->start(1000);//定时1s更新一次

}

/**
功能：刷新串口槽函数
修改：sjm
参数：-
返回值：-
**/
void RtcMessage::updateSerialData()
{
    // 检查串口是否仍然存在
    QList<QSerialPortInfo> availablePorts = QSerialPortInfo::availablePorts();

    // 如果串口不存在，清空com_connect控件的串口并将led控件变为红色
    if (availablePorts.isEmpty())
    {
        ui->com_connect->clear();
        ui->led->setStyleSheet("QLabel { border-radius: 10px; border: 1px solid #000; background-color:rgb(255,0,0); }");
        labe4->setText("USB未连接，请连接USB设备");
    }
    else
    {
        // 串口存在时检查是否打开
        if (serialPort->isOpen())
        {
            // 串口打开，则将led控件变为绿色
            ui->led->setStyleSheet("QLabel { border-radius: 10px; border: 1px solid #000; background-color:rgb(0,255,0); }");
            labe4->setText("USB设备已连接");
        }
        else
        {
            // 串口存在但未打开,则将led控件变为黄灯
            ui->led->setStyleSheet("QLabel { border-radius: 10px; border: 1px solid #000; background-color:rgb(255,255,0); }");
            labe4->setText("USB设备已连接，但未打开");
        }
    }
}


/**
功能：切换串口槽函数
修改：sjm
参数：-
返回值：-
**/
void RtcMessage::changeSerialPort(const QString& portName)
{
    if (serialPort->isOpen())
    {
        // 关闭当前串口
        serialPort->close();
    }

    // 根据选中的串口名称打开新的串口
    serialPort->setPortName(portName);
    if (serialPort->open(QIODevice::ReadWrite))
    {
        serialPort->setBaudRate(QSerialPort::Baud19200);
        serialPort->setDataBits(QSerialPort::Data8);
        serialPort->setParity(QSerialPort::NoParity);
        serialPort->setStopBits(QSerialPort::OneStop);
        serialPort->setFlowControl(QSerialPort::NoFlowControl);
        qDebug() << "串口打开成功：" << ui->com_connect->currentText();
    }
    else
    {
        qDebug() << "串口打开失败：" << ui->com_connect->currentText();
    }

}




/**
功能：器件选择界面槽函数
修改：sjm
参数：-
返回值：-
**/
void RtcMessage::skip(void)
{
    SelectDevice *dvesel=new SelectDevice;
    dvesel->show();
    this->close();
    serialPort->close();

}


/**
功能：刷新串口槽函数
修改：sjm
参数：-
返回值：-
**/
void RtcMessage::ReCom(void)
{
    ui->com_connect->clear();//每次进来点击清空串口
    QList<QSerialPortInfo> serialList = QSerialPortInfo::availablePorts();  //获取可用的串口列表
    for(QSerialPortInfo serialinfo : serialList){   //迭代器迭代遍历
        qDebug() << serialinfo.portName();
        ui->com_connect->addItem(serialinfo.portName()); //将可用的串口添加到串口中
    }

    if (serialPort->open(QIODevice::ReadWrite))
    {
        serialPort->setBaudRate(QSerialPort::Baud19200);
        serialPort->setDataBits(QSerialPort::Data8);
        serialPort->setParity(QSerialPort::NoParity);
        serialPort->setStopBits(QSerialPort::OneStop);
        serialPort->setFlowControl(QSerialPort::NoFlowControl);
         qDebug() << "串口刷新成功：" << ui->com_connect->currentText();
    }
    else
    {
        qDebug() << "串口刷新失败：" << ui->com_connect->currentText();
    }

}

/**
功能：调用外部软件槽函数
修改：sjm
参数：-
返回值：-
**/
void RtcMessage::Computer_time(void)
{
    // 创建一个QProcess对象
    QProcess process;
    // 设置外部应用程序的路径和命令行参数
    QString str = "C:\\Qt Project\\Clock_Debug_SystemV5.5_com\\AboutTime.exe";
    QStringList arg;
    arg << "arg1" << "arg2";

    // 启动外部应用程序
    process.startDetached(str, arg);
    process.waitForFinished();

    // 获取外部应用程序的输出
    QByteArray output = process.readAllStandardOutput();
    QString outputString(output);

    // 打印输出
    qDebug() << "Output: " << outputString;


}

/**
功能：创建并显示注意事项窗口
修改：sjm
参数：-
返回值：-
**/
void RtcMessage::notices(void)
{

    notice *noticeui=new notice;
    noticeui->show();

}

/**
功能：网页跳转
修改：sjm
参数：-
返回值：-
**/
void RtcMessage::homepages(void)
{

    QString url = "https://www.whwave.com.cn/"; // 指定要跳转的网页链接
    QDesktopServices::openUrl(QUrl(url)); // 打开指定的网页链接

}

/**
功能：创建并显示联系我们窗口
修改：sjm
参数：-
返回值：-
**/
void RtcMessage::relations()
{

    relation *relationui=new relation;
    relationui->show();

}

/**
功能：创建并显示历史版本窗口
修改：sjm
参数：-
返回值：-
**/
void RtcMessage::historys()
{

    history *historyui=new history;
    historyui->show();

}


/*********************************************************RTC模型参数设置***************************************************************/
/**
功能：芯片寄存器显示和按键设置功能(以下函数设置和显示信息的方法一致)--分为sdxxxx()-创建数据库模型,sdxxxxModelselt()-显示数据库模型
修改：sjm
参数：-
返回值：-
**/
//3068
void RtcMessage::sd3068(void)
{
        //刷新数据
        ui->rtctype->update();
        //设置按钮文本
        ui->R00H->setText("读寄存器00H-1FH");
        ui->R20H->setText("读寄存器20H-3FH");
        ui->R40H->setText("读寄存器40H-5FH");
        ui->R60H->setText("读寄存器60H-79H");

        ui->W00H->setText("写寄存器00H-06H");
        ui->W07H->setText("写寄存器07H-1FH");
        ui->W2CH->setText("写寄存器2CH-3FH");
        ui->W40H->setText("写寄存器40H-5FH");
        ui->W60H->setText("写寄存器60H-79H");

        //创建数据库
        SD3068=new RTCdata;
        //引用数据库
        SD3068->SD3068CreateDB();
        // 创建数据模型
        sd3068model = new QSqlTableModel(this);
        sd3068model->setTable("SD3068RTCreg");
        sd3068model->setEditStrategy(QSqlTableModel::OnFieldChange); // 设置编辑策略
        // 设置TableView控件使用数据模型
        ui->tableView->setModel(sd3068model);
        CustomItemDelegate *delegate = new CustomItemDelegate;//设置编辑状态
        ui->tableView->setItemDelegate(delegate);
        sd3068model->select(); // 从数据库中获取数据

//        // 将前64行的奇数行、前8列的单元格设置为0
//        for (int row = 0; row <= 63; row++) {
//            if (row % 2 != 0) {
//                for (int col = 0; col < 9; col++) {
//                    QModelIndex index =  sd3068model->index(row, col);
//                     sd3068model->setData(index, 0);
//                }
//            }
//        }

//        for (int row = 76; row <= 145; row++) {
//                for (int col = 0; col < 8; col++) {
//                    QModelIndex index =  sd3068model->index(row, col);
//                     sd3068model->setData(index, 0);
//                }

//        }

        //将前64行的奇数行、3~11列的单元格设置为0--每次重新选择型号后都会将所有寄存器写入0
        for (int row = 0; row <= 153; row++) {
            if (row <= 63 && row % 2 != 0) {
                for (int col = 3; col <= 11; col++) {
                    QModelIndex index =  sd3068model->index(row, col);
                     sd3068model->setData(index, 0);
                }//将64~153行、3~11列的单元格设置为0
            }else if (row >= 64 && row <= 153) {
                for (int col = 3; col <= 11; col++) {
                    QModelIndex index =  sd3068model->index(row, col);
                     sd3068model->setData(index, 0);
                }
            }
        }

        // 提交更改到数据库中
        if (! sd3068model->submitAll()) {
            qDebug() << "Failed to submit changes to database:" <<  sd3068model->lastError().text();
            QMessageBox::information(this,"加载错误",sd3068model->lastError().text());

        }

//        connect(sd3068model, &QSqlTableModel::dataChanged, [=](const QModelIndex &topLeft, const QModelIndex &bottomRight)
//        {
//            for (int row = topLeft.row(); row <= bottomRight.row(); ++row)
//            {
//                int column = 11; // 检查第12列（index为11）的数据变化
//                if (topLeft.column() <= column && bottomRight.column() >= column)
//                {
//                    for (int outerRow = 1; outerRow <= 153; outerRow ++)
//                    {
//                        QString hexInput = sd3068model->data(sd3068model->index(outerRow, 11)).toString();
//                        if (outerRow <= 63 && outerRow % 2 != 0)
//                        {
//                            for (int col = 3; col < 11; col++)
//                            {
//                                if(hexInput != "0")
//                                {
//                                    bool ok;
//                                    int decimalValue = hexInput.toInt(&ok, 16); // 将16进制字符串转换为整数
//                                    QString binaryValue1 = QString::number(decimalValue, 2).rightJustified(8, '0'); // 将整数转换为8位二进制字符串
//                                    sd3068model->setData(sd3068model->index(outerRow, col), binaryValue1.mid(col - 3, 1));
//                                }
//                            }
//                        }else if (outerRow >= 64 && outerRow <= 153)
//                        {
//                            for (int col = 3; col < 11; col++)
//                            {
//                                if(hexInput != "0")
//                                {
//                                    bool ok;
//                                    int decimalValue = hexInput.toInt(&ok, 16); // 将16进制字符串转换为整数
//                                    QString binaryValue1 = QString::number(decimalValue, 2).rightJustified(8, '0'); // 将整数转换为8位二进制字符串
//                                    sd3068model->setData(sd3068model->index(outerRow, col), binaryValue1.mid(col - 3, 1));
//                                }
//                            }

//                        }

//                    }
//                }
//            }
//        });

        // 获取表头视图--设置表头宽度
        QHeaderView *headerView = ui->tableView->horizontalHeader();
        headerView->resizeSection(0,35); // 第一列宽度为35像素
        headerView->resizeSection(1,94);
        headerView->resizeSection(2,88);
        headerView->resizeSection(3,70);
        headerView->resizeSection(4,70);
        headerView->resizeSection(5,70);
        headerView->resizeSection(6,70);
        headerView->resizeSection(7,70);
        headerView->resizeSection(8,70);
        headerView->resizeSection(9,70);
        headerView->resizeSection(10,70);
        headerView->resizeSection(11,60);
        headerView->resizeSection(12,79);
        headerView->resizeSection(13,89);
        headerView->resizeSection(14,0);
}

/**
功能：显示数据库模型
修改：sjm
参数：-
返回值：-
**/
void RtcMessage::sd3068Modelselt(void)
{

    // 创建数据模型
    sd3068model = new QSqlTableModel(this);
    sd3068model->setTable("SD3068RTCreg");
    sd3068model->setEditStrategy(QSqlTableModel::OnFieldChange); // 设置编辑策略
    sd3068model->select(); // 从数据库中获取数据
    // 设置TableView控件使用数据模型
    ui->tableView->setModel(sd3068model);

//        connect(sd3068model, &QSqlTableModel::dataChanged, [=](const QModelIndex &topLeft, const QModelIndex &bottomRight)
//        {
//            for (int row = topLeft.row(); row <= bottomRight.row(); ++row)
//            {
//                int column = 11; // 检查第12列（index为11）的数据变化
//                if (topLeft.column() <= column && bottomRight.column() >= column)
//                {
//                    for (int outerRow = 1; outerRow <= 153; outerRow ++)
//                    {
//                        QString hexInput = sd3068model->data(sd3068model->index(outerRow, 11)).toString();
//                        if (outerRow <= 63 && outerRow % 2 != 0)
//                        {
//                            for (int col = 3; col < 11; col++)
//                            {
//                                if(hexInput != "0")
//                                {
//                                    bool ok;
//                                    int decimalValue = hexInput.toInt(&ok, 16); // 将16进制字符串转换为整数
//                                    QString binaryValue1 = QString::number(decimalValue, 2).rightJustified(8, '0'); // 将整数转换为8位二进制字符串
//                                    sd3068model->setData(sd3068model->index(outerRow, col), binaryValue1.mid(col - 3, 1));
//                                }
//                            }
//                        }else if (outerRow >= 64 && outerRow <= 153)
//                        {
//                            for (int col = 3; col < 11; col++)
//                            {
//                                if(hexInput != "0")
//                                {
//                                    bool ok;
//                                    int decimalValue = hexInput.toInt(&ok, 16); // 将16进制字符串转换为整数
//                                    QString binaryValue1 = QString::number(decimalValue, 2).rightJustified(8, '0'); // 将整数转换为8位二进制字符串
//                                    sd3068model->setData(sd3068model->index(outerRow, col), binaryValue1.mid(col - 3, 1));
//                                }
//                            }

//                        }

//                    }
//                }
//            }
//        });
}

//8563
void RtcMessage::sd8563(void)
{
        ui->rtctype->update();

        ui->R00H->setText("读寄存器00H-0FH");

        ui->W00H->setText("写寄存器02H-08H");
        ui->W07H->setText("写寄存器00H-0FH");

        SD8563=new RTCdata;
        SD8563->SD8563CreateDB();
        sd8563model = new QSqlTableModel(this);
        sd8563model->setTable("SD8563RTCreg");
        sd8563model->setEditStrategy(QSqlTableModel::OnFieldChange); // 设置编辑策略
        // 设置TableView控件使用数据模型
        ui->tableView->setModel(sd8563model);
        CustomItemDelegate *delegate = new CustomItemDelegate;
        ui->tableView->setItemDelegate(delegate);
        sd8563model->select(); // 从数据库中获取数据

        // 将前64行的奇数行、前8列的单元格设置为0
        for (int row = 0; row <= 63; row++) {
            if (row % 2 != 0) {
                for (int col = 3; col < 12; col++) {
                    QModelIndex index =  sd8563model->index(row, col);
                     sd8563model->setData(index, 0);
                }
            }
        }
        // 提交更改到数据库中
        if (! sd8563model->submitAll()) {
            qDebug() << "Failed to submit changes to database:" <<  sd8563model->lastError().text();
            QMessageBox::information(this,"加载错误",sd8563model->lastError().text());

        }

        // 获取表头视图
        QHeaderView *headerView = ui->tableView->horizontalHeader();
        headerView->resizeSection(0,35); // 第一列宽度为35像素
        headerView->resizeSection(1,94);
        headerView->resizeSection(2,88);
        headerView->resizeSection(3,70);
        headerView->resizeSection(4,70);
        headerView->resizeSection(5,70);
        headerView->resizeSection(6,70);
        headerView->resizeSection(7,70);
        headerView->resizeSection(8,70);
        headerView->resizeSection(9,70);
        headerView->resizeSection(10,70);
        headerView->resizeSection(11,60);
        headerView->resizeSection(12,79);
        headerView->resizeSection(13,89);
        headerView->resizeSection(14,0);
}


void RtcMessage::sd8563Modelselt(void)
{
    // 创建数据模型
    sd8563model = new QSqlTableModel(this);
    sd8563model->setTable("SD8563RTCreg");
    sd8563model->setEditStrategy(QSqlTableModel::OnFieldChange); // 设置编辑策略
    sd8563model->select(); // 从数据库中获取数据
    // 设置TableView控件使用数据模型
    ui->tableView->setModel(sd8563model);
}

//8825
void RtcMessage::sd8825()
{
    ui->rtctype->update();

    ui->R60H->setText("");
    ui->R40H->setText("读寄存器FBH-FFH");
    ui->R20H->setText("读寄存器6DH-B9H");
    ui->R00H->setText("读寄存器00H-5AH");

    ui->W60H->setText("");
    ui->W40H->setText("写寄存器FBH-FFH");
    ui->W2CH->setText("写寄存器6DH-B9H");
    ui->W07H->setText("写寄存器00H-5AH");
    ui->W00H->setText("写寄存器00H-07H");

    SD8825=new RTCdata;
    SD8825->SD8825CreateDB();
    // 创建数据模型
    sd8825model = new QSqlTableModel(this);
    sd8825model->setTable("SD8825RTCreg");
    sd8825model->setEditStrategy(QSqlTableModel::OnFieldChange); // 设置编辑策略
    // 设置TableView控件使用数据模型
    ui->tableView->setModel(sd8825model);
    CustomItemDelegate8825 *delegate = new CustomItemDelegate8825;//设置编辑状态
    ui->tableView->setItemDelegate(delegate);
    sd8825model->select(); // 从数据库中获取数据

    for (int row = 0; row <= 203; row++) {
        if (row % 2 != 0) {
            for (int col = 3; col < 12; col++) {
                QModelIndex index =  sd8825model->index(row, col);
                 sd8825model->setData(index, 0);
            }
        }
    }
    // 提交更改到数据库中
    if (! sd8825model->submitAll()) {
        qDebug() << "Failed to submit changes to database:" <<  sd8825model->lastError().text();
        QMessageBox::information(this,"加载错误",sd8825model->lastError().text());

    }

    // 获取表头视图
    QHeaderView *headerView = ui->tableView->horizontalHeader();
    headerView->resizeSection(0,35); // 第一列宽度为35像素
    headerView->resizeSection(1,100);
    headerView->resizeSection(2,82);
    headerView->resizeSection(3,70);
    headerView->resizeSection(4,70);
    headerView->resizeSection(5,70);
    headerView->resizeSection(6,70);
    headerView->resizeSection(7,70);
    headerView->resizeSection(8,70);
    headerView->resizeSection(9,70);
    headerView->resizeSection(10,70);
    headerView->resizeSection(11,60);
    headerView->resizeSection(12,79);
    headerView->resizeSection(13,89);
    headerView->resizeSection(14,0);
}

void RtcMessage::sd8825Modelselt()
{
    ui->rtctype->update();
    SD8825=new RTCdata;
    SD8825->SD8825CreateDB();
    // 创建数据模型
    sd8825model = new QSqlTableModel(this);
    sd8825model->setTable("SD8825RTCreg");
    sd8825model->setEditStrategy(QSqlTableModel::OnFieldChange); // 设置编辑策略
    sd8825model->select(); // 从数据库中获取数据
    // 设置TableView控件使用数据模型
    ui->tableView->setModel(sd8825model);
}

//8804
void RtcMessage::sd8804()
{
    ui->rtctype->update();

    ui->R60H->setText("读寄存器FBH-FFH");
    ui->R40H->setText("读寄存器6DH-D9H");
    ui->R20H->setText("读寄存器10H-5DH");
    ui->R00H->setText("读寄存器00H-0FH");

//    ui->refresh->setText("写保留ID寄存器");
    ui->W60H->setText("写寄存器FB-FFH");
    ui->W40H->setText("写RAM寄存器");
    ui->W2CH->setText("写寄存器10H-5DH");
    ui->W07H->setText("写寄存器00H-0FH");
    ui->W00H->setText("写寄存器00H-06H");

    ui->rtctype->update();
    SD8804=new RTCdata;
    SD8804->SD8804CreateDB();
    // 创建数据模型
    sd8804model = new QSqlTableModel(this);
    sd8804model->setTable("SD8804RTCreg");
    sd8804model->setEditStrategy(QSqlTableModel::OnFieldChange); // 设置编辑策略
    // 设置TableView控件使用数据模型
    ui->tableView->setModel(sd8804model);
    CustomItemDelegate8804 *delegate = new CustomItemDelegate8804;//设置编辑状态
    ui->tableView->setItemDelegate(delegate);
    sd8804model->select(); // 从数据库中获取数据

    //刷新数据
    for (int row = 0; row <= 196; row++) {
        if (row <= 79 && row % 2 != 0) {
            for (int col = 3; col < 12; col++) {
                QModelIndex index =  sd8804model->index(row, col);
                 sd8804model->setData(index, 0);
            }
        }else if (row >= 80 && row <= 188) {
            for (int col = 3; col < 12; col++) {
                QModelIndex index =  sd8804model->index(row, col);
                 sd8804model->setData(index, 0);
            }
        }else if(row >= 190 && row % 2 == 0){
            for (int col = 3; col < 12; col++) {
                QModelIndex index =  sd8804model->index(row, col);
                 sd8804model->setData(index, 0);
            }
        }
    }

    // 提交更改到数据库中
    if (! sd8804model->submitAll()) {
        qDebug() << "Failed to submit changes to database:" <<  sd8804model->lastError().text();
        QMessageBox::information(this,"加载错误",sd8804model->lastError().text());

    }

    // 获取表头视图
    QHeaderView *headerView = ui->tableView->horizontalHeader();
    headerView->resizeSection(0,35); // 第一列宽度为35像素
    headerView->resizeSection(1,94);
    headerView->resizeSection(2,110);
    headerView->resizeSection(3,70);
    headerView->resizeSection(4,70);
    headerView->resizeSection(5,70);
    headerView->resizeSection(6,70);
    headerView->resizeSection(7,70);
    headerView->resizeSection(8,70);
    headerView->resizeSection(9,70);
    headerView->resizeSection(10,70);
    headerView->resizeSection(11,60);
    headerView->resizeSection(12,79);
    headerView->resizeSection(13,67);
    headerView->resizeSection(14,0);
}

void RtcMessage::sd8804Modelselt()
{
    ui->rtctype->update();
    SD8804=new RTCdata;
    SD8804->SD8804CreateDB();
    // 创建数据模型
    sd8804model = new QSqlTableModel(this);
    sd8804model->setTable("SD8804RTCreg");
    sd8804model->setEditStrategy(QSqlTableModel::OnFieldChange); // 设置编辑策略
    sd8804model->select(); // 从数据库中获取数据
    // 设置TableView控件使用数据模型
    ui->tableView->setModel(sd8804model);
}

//8840
void RtcMessage::sd8840()
{
    ui->rtctype->update();

    ui->R60H->setText("");
    ui->R40H->setText("读寄存器6DH-7FH");
    ui->R20H->setText("读寄存器42H-5AH");
    ui->R00H->setText("读寄存器00H-0FH");

    ui->W60H->setText("");
    ui->W40H->setText("写寄存器6DH-7FH");
    ui->W2CH->setText("写寄存器42H-5AH");
    ui->W07H->setText("写寄存器00H-0FH");
    ui->W00H->setText("写寄存器00H-06H");

    ui->rtctype->update();
    SD8840=new RTCdata;
    SD8840->SD8840CreateDB();
    // 创建数据模型
    sd8840model = new QSqlTableModel(this);
    sd8840model->setTable("SD8840RTCreg");
    sd8840model->setEditStrategy(QSqlTableModel::OnFieldChange); // 设置编辑策略
    // 设置TableView控件使用数据模型
    ui->tableView->setModel(sd8840model);
    CustomItemDelegate8825 *delegate = new CustomItemDelegate8825;//设置编辑状态
    ui->tableView->setItemDelegate(delegate);
    sd8840model->select(); // 从数据库中获取数据

    for (int row = 0; row <= 89; row++) {
        if (row % 2 != 0) {
            for (int col = 3; col < 12; col++) {
                QModelIndex index =  sd8840model->index(row, col);
                 sd8840model->setData(index, 0);
            }
        }
    }
    // 提交更改到数据库中
    if (! sd8840model->submitAll()) {
        qDebug() << "Failed to submit changes to database:" <<  sd8840model->lastError().text();
        QMessageBox::information(this,"加载错误",sd8840model->lastError().text());

    }

    // 获取表头视图
    QHeaderView *headerView = ui->tableView->horizontalHeader();    
    headerView->resizeSection(0,35); // 第一列宽度为35像素
    headerView->resizeSection(1,100);
    headerView->resizeSection(2,88);
    headerView->resizeSection(3,70);
    headerView->resizeSection(4,70);
    headerView->resizeSection(5,70);
    headerView->resizeSection(6,70);
    headerView->resizeSection(7,70);
    headerView->resizeSection(8,70);
    headerView->resizeSection(9,70);
    headerView->resizeSection(10,70);
    headerView->resizeSection(11,60);
    headerView->resizeSection(12,79);
    headerView->resizeSection(13,89);
    headerView->resizeSection(14,0);
}

void RtcMessage::sd8840Modelselt()
{
    ui->rtctype->update();
    SD8840=new RTCdata;
    SD8840->SD8840CreateDB();
    // 创建数据模型
    sd8840model = new QSqlTableModel(this);
    sd8840model->setTable("SD8840RTCreg");
    sd8840model->setEditStrategy(QSqlTableModel::OnFieldChange); // 设置编辑策略
    sd8840model->select(); // 从数据库中获取数据
    // 设置TableView控件使用数据模型
    ui->tableView->setModel(sd8840model);
}

//8902
void RtcMessage::sd8902()
{
    ui->rtctype->update();

    ui->R60H->setText("读RAM寄存器");
    ui->R40H->setText("读寄存器AFH-BDH");
    ui->R20H->setText("读寄存器8FH-A1H");
    ui->R00H->setText("读寄存器81H-8DH");

    ui->W60H->setText("写RAM寄存器");
    ui->W40H->setText("写寄存器BAH-BCH");
    ui->W2CH->setText("写寄存器B2H-B4H");
    ui->W07H->setText("写寄存器80H-90H");
    ui->W00H->setText("写寄存器80H-8CH");

    SD8902=new RTCdata;
    SD8902->SD8902CreateDB();
    sd8902model = new QSqlTableModel(this);
    sd8902model->setTable("SD8902RTCreg");
    sd8902model->setEditStrategy(QSqlTableModel::OnFieldChange); // 设置编辑策略
    // 设置TableView控件使用数据模型
    ui->tableView->setModel(sd8902model);
    CustomItemDelegate8825 *delegate = new CustomItemDelegate8825;//设置编辑状态
    ui->tableView->setItemDelegate(delegate);
    sd8902model->select(); // 从数据库中获取数据

    for (int row = 0; row <= 111; row++) {
        if (row % 2 != 0) {
            for (int col = 3; col < 12; col++) {
                QModelIndex index =  sd8902model->index(row, col);
                 sd8902model->setData(index, 0);
            }
        }
    }
    // 提交更改到数据库中
    if (! sd8902model->submitAll()) {
        qDebug() << "Failed to submit changes to database:" <<  sd8902model->lastError().text();
        QMessageBox::information(this,"加载错误",sd8902model->lastError().text());

    }

    // 获取表头视图
    QHeaderView *headerView = ui->tableView->horizontalHeader();
    headerView->resizeSection(0,64); // 第一列宽度为35像素
    headerView->resizeSection(1,97);
    headerView->resizeSection(2,85);
    headerView->resizeSection(3,70);
    headerView->resizeSection(4,70);
    headerView->resizeSection(5,70);
    headerView->resizeSection(6,70);
    headerView->resizeSection(7,70);
    headerView->resizeSection(8,70);
    headerView->resizeSection(9,70);
    headerView->resizeSection(10,70);
    headerView->resizeSection(11,60);
    headerView->resizeSection(12,79);
    headerView->resizeSection(13,60);
    headerView->resizeSection(14,0);
}

void RtcMessage::sd8902Modelselt()
{
    // 创建数据模型
    sd8902model = new QSqlTableModel(this);
    sd8902model->setTable("SD8902RTCreg");
    sd8902model->setEditStrategy(QSqlTableModel::OnFieldChange); // 设置编辑策略
    sd8902model->select(); // 从数据库中获取数据
    // 设置TableView控件使用数据模型
    ui->tableView->setModel(sd8902model);
}

//8931
void RtcMessage::sd8931()
{
    ui->rtctype->update();

    ui->R60H->setText("");
    ui->R40H->setText("读寄存器FBH-FFH");
    ui->R20H->setText("读寄存器6CH-B9H");
    ui->R00H->setText("读寄存器00H-58H");

    ui->W60H->setText("");
    ui->W40H->setText("写寄存器FBH-FFH");
    ui->W2CH->setText("写寄存器6CH-B9H");
    ui->W07H->setText("写寄存器00H-58H");
    ui->W00H->setText("写寄存器00H-06H");

    SD8931=new RTCdata;
    SD8931->SD8931CreateDB();
    sd8931model = new QSqlTableModel(this);
    sd8931model->setTable("SD8931RTCreg");
    sd8931model->setEditStrategy(QSqlTableModel::OnFieldChange); // 设置编辑策略
    // 设置TableView控件使用数据模型
    ui->tableView->setModel(sd8931model);
    CustomItemDelegate8825 *delegate = new CustomItemDelegate8825;//设置编辑状态
    ui->tableView->setItemDelegate(delegate);
    sd8931model->select(); // 从数据库中获取数据

    for (int row = 0; row <= 207; row++) {
        if (row % 2 != 0) {
            for (int col = 3; col < 12; col++) {
                QModelIndex index =  sd8931model->index(row, col);
                 sd8931model->setData(index, 0);
            }
        }
    }
    // 提交更改到数据库中
    if (! sd8931model->submitAll()) {
        qDebug() << "Failed to submit changes to database:" <<  sd8931model->lastError().text();
        QMessageBox::information(this,"加载错误",sd8931model->lastError().text());

    }

    // 获取表头视图
    QHeaderView *headerView = ui->tableView->horizontalHeader();
    headerView->resizeSection(0,35); // 第一列宽度为35像素
    headerView->resizeSection(1,98);
    headerView->resizeSection(2,67);
    headerView->resizeSection(3,70);
    headerView->resizeSection(4,70);
    headerView->resizeSection(5,70);
    headerView->resizeSection(6,70);
    headerView->resizeSection(7,70);
    headerView->resizeSection(8,87);
    headerView->resizeSection(9,87);
    headerView->resizeSection(10,87);
    headerView->resizeSection(11,57);
    headerView->resizeSection(12,79);
    headerView->resizeSection(13,58);
    headerView->resizeSection(14,0);
}

void RtcMessage::sd8931Modelselt()
{
    // 创建数据模型
    sd8931model = new QSqlTableModel(this);
    sd8931model->setTable("SD8931RTCreg");
    sd8931model->setEditStrategy(QSqlTableModel::OnFieldChange); // 设置编辑策略
    sd8931model->select(); // 从数据库中获取数据
    // 设置TableView控件使用数据模型
    ui->tableView->setModel(sd8931model);
}

//8810
void RtcMessage::sd8810()
{
    ui->rtctype->update();

    ui->R60H->setText("读寄存器FBH-FFH");
    ui->R40H->setText("读寄存器7CH-B9H");
    ui->R20H->setText("读寄存器20H-5AH");
    ui->R00H->setText("读寄存器10H-1FH");

    ui->W60H->setText("写寄存器FBH-FFH");
    ui->W40H->setText("写RAM寄存器");
    ui->W2CH->setText("写寄存器20H-5AH");
    ui->W07H->setText("写寄存器10H-1FH");
    ui->W00H->setText("写寄存器10H-16H");

    SD8810=new RTCdata;
    SD8810->SD8810CreateDB();
    sd8810model = new QSqlTableModel(this);
    sd8810model->setTable("SD8810RTCreg");
    sd8810model->setEditStrategy(QSqlTableModel::OnFieldChange); // 设置编辑策略
    // 设置TableView控件使用数据模型
    ui->tableView->setModel(sd8810model);
    CustomItemDelegate8825 *delegate = new CustomItemDelegate8825;//设置编辑状态
    ui->tableView->setItemDelegate(delegate);
    sd8810model->select(); // 从数据库中获取数据

    for (int row = 0; row <= 207; row++) {
        if (row % 2 != 0) {
            for (int col = 3; col < 12; col++) {
                QModelIndex index =  sd8810model->index(row, col);
                 sd8810model->setData(index, 0);
            }
        }
    }
    // 提交更改到数据库中
    if (! sd8810model->submitAll()) {
        qDebug() << "Failed to submit changes to database:" <<  sd8810model->lastError().text();
        QMessageBox::information(this,"加载错误",sd8810model->lastError().text());

    }

    // 获取表头视图
    QHeaderView *headerView = ui->tableView->horizontalHeader();
    headerView->resizeSection(0,35); // 第一列宽度为35像素
    headerView->resizeSection(1,104);
    headerView->resizeSection(2,88);
    headerView->resizeSection(3,70);
    headerView->resizeSection(4,70);
    headerView->resizeSection(5,70);
    headerView->resizeSection(6,70);
    headerView->resizeSection(7,70);
    headerView->resizeSection(8,70);
    headerView->resizeSection(9,70);
    headerView->resizeSection(10,70);
    headerView->resizeSection(11,60);
    headerView->resizeSection(12,79);
    headerView->resizeSection(13,79);
    headerView->resizeSection(14,0);
}

void RtcMessage::sd8810Modelselt()
{
    // 创建数据模型
    sd8810model = new QSqlTableModel(this);
    sd8810model->setTable("SD8810RTCreg");
    sd8810model->setEditStrategy(QSqlTableModel::OnFieldChange); // 设置编辑策略
    sd8810model->select(); // 从数据库中获取数据
    // 设置TableView控件使用数据模型
    ui->tableView->setModel(sd8810model);
}

//8939
void RtcMessage::sd8939()
{
    ui->rtctype->update();

    ui->R60H->setText("");
    ui->R40H->setText("读寄存器FBH-FFH");
    ui->R20H->setText("读寄存器6CH-B9H");
    ui->R00H->setText("读寄存器00H-57H");

    ui->W60H->setText("");
    ui->W40H->setText("写寄存器FBH-FFH");
    ui->W2CH->setText("写寄存器6CH-B9H");
    ui->W07H->setText("写寄存器00H-57H");
    ui->W00H->setText("写寄存器00H-06H");

    SD8939=new RTCdata;
    SD8939->SD8939CreateDB();
    sd8939model = new QSqlTableModel(this);
    sd8939model->setTable("SD8939RTCreg");
    sd8939model->setEditStrategy(QSqlTableModel::OnFieldChange); // 设置编辑策略
    // 设置TableView控件使用数据模型
    ui->tableView->setModel(sd8939model);
    CustomItemDelegate8825 *delegate = new CustomItemDelegate8825;//设置编辑状态
    ui->tableView->setItemDelegate(delegate);
    sd8939model->select(); // 从数据库中获取数据

    for (int row = 0; row <= 207; row++) {
        if (row % 2 != 0) {
            for (int col = 3; col < 12; col++) {
                QModelIndex index =  sd8939model->index(row, col);
                 sd8939model->setData(index, 0);
            }
        }
    }
    // 提交更改到数据库中
    if (! sd8939model->submitAll()) {
        qDebug() << "Failed to submit changes to database:" <<  sd8939model->lastError().text();
        QMessageBox::information(this,"加载错误",sd8939model->lastError().text());

    }

    // 获取表头视图
    QHeaderView *headerView = ui->tableView->horizontalHeader();
    headerView->resizeSection(0,35); // 第一列宽度为35像素
    headerView->resizeSection(1,98);
    headerView->resizeSection(2,67);
    headerView->resizeSection(3,70);
    headerView->resizeSection(4,70);
    headerView->resizeSection(5,70);
    headerView->resizeSection(6,70);
    headerView->resizeSection(7,70);
    headerView->resizeSection(8,87);
    headerView->resizeSection(9,87);
    headerView->resizeSection(10,87);
    headerView->resizeSection(11,57);
    headerView->resizeSection(12,79);
    headerView->resizeSection(13,58);
    headerView->resizeSection(14,0);
}

void RtcMessage::sd8939Modelselt()
{
    // 创建数据模型
    sd8939model = new QSqlTableModel(this);
    sd8939model->setTable("SD8939RTCreg");
    sd8939model->setEditStrategy(QSqlTableModel::OnFieldChange); // 设置编辑策略
    sd8939model->select(); // 从数据库中获取数据
    // 设置TableView控件使用数据模型
    ui->tableView->setModel(sd8939model);
}

//8800
void RtcMessage::sd8800()
{
    ui->rtctype->update();

    ui->R60H->setText("读寄存器F2H-FFH");
    ui->R40H->setText("读寄存器6DH-D9H");
    ui->R20H->setText("读寄存器17H-5DH");
    ui->R00H->setText("读寄存器00H-0FH");

//    ui->refresh->setText("写保留ID寄存器");
    ui->W60H->setText("写寄存器F2-FFH");
    ui->W40H->setText("写RAM寄存器");
    ui->W2CH->setText("写寄存器17H-5DH");
    ui->W07H->setText("写寄存器00H-0FH");
    ui->W00H->setText("写寄存器00H-06H");

    SD8800=new RTCdata;
    SD8800->SD8800CreateDB();
    sd8800model = new QSqlTableModel(this);
    sd8800model->setTable("SD8800RTCreg");
    sd8800model->setEditStrategy(QSqlTableModel::OnFieldChange); // 设置编辑策略
    // 设置TableView控件使用数据模型
    ui->tableView->setModel(sd8800model);
    CustomItemDelegate8800 *delegate = new CustomItemDelegate8800;//设置编辑状态
    ui->tableView->setItemDelegate(delegate);
    sd8800model->select(); // 从数据库中获取数据

    //刷新数据
    for (int row = 0; row <= 188; row++) {
        if (row <= 53 && row % 2 != 0) {
            for (int col = 3; col < 12; col++) {
                QModelIndex index =  sd8800model->index(row, col);
                 sd8800model->setData(index, 0);
            }
        }else if (row >= 54 && row <= 162) {
            for (int col = 3; col < 12; col++) {
                QModelIndex index =  sd8800model->index(row, col);
                 sd8800model->setData(index, 0);
            }
        }else if(row >= 163 && row % 2 == 0){
            for (int col = 3; col < 12; col++) {
                QModelIndex index =  sd8800model->index(row, col);
                 sd8800model->setData(index, 0);
            }
        }
    }

    // 提交更改到数据库中
    if (! sd8800model->submitAll()) {
        qDebug() << "Failed to submit changes to database:" <<  sd8800model->lastError().text();
        QMessageBox::information(this,"加载错误",sd8800model->lastError().text());

    }

    // 获取表头视图
    QHeaderView *headerView = ui->tableView->horizontalHeader();
    headerView->resizeSection(0,35); // 第一列宽度为35像素
    headerView->resizeSection(1,98);
    headerView->resizeSection(2,67);
    headerView->resizeSection(3,70);
    headerView->resizeSection(4,70);
    headerView->resizeSection(5,70);
    headerView->resizeSection(6,70);
    headerView->resizeSection(7,70);
    headerView->resizeSection(8,87);
    headerView->resizeSection(9,87);
    headerView->resizeSection(10,87);
    headerView->resizeSection(11,57);
    headerView->resizeSection(12,79);
    headerView->resizeSection(13,58);
    headerView->resizeSection(14,0);
}

void RtcMessage::sd8800Modelselt()
{
    // 创建数据模型
    sd8800model = new QSqlTableModel(this);
    sd8800model->setTable("SD8800RTCreg");
    sd8800model->setEditStrategy(QSqlTableModel::OnFieldChange); // 设置编辑策略
    sd8800model->select(); // 从数据库中获取数据
    // 设置TableView控件使用数据模型
    ui->tableView->setModel(sd8800model);
}

//8200
void RtcMessage::sd8200()
{
    ui->rtctype->update();

    ui->R60H->setText("");
    ui->R40H->setText("读寄存器FBH-FFH");
    ui->R20H->setText("读寄存器6CH-B9H");
    ui->R00H->setText("读寄存器00H-5BH");

    ui->W60H->setText("");
    ui->W40H->setText("写寄存器FBH-FFH");
    ui->W2CH->setText("写寄存器6CH-B9H");
    ui->W07H->setText("写寄存器00H-5BH");
    ui->W00H->setText("写寄存器00H-06H");

    SD8200=new RTCdata;
    SD8200->SD8200CreateDB();
    sd8200model = new QSqlTableModel(this);
    sd8200model->setTable("SD8200RTCreg");
    sd8200model->setEditStrategy(QSqlTableModel::OnFieldChange); // 设置编辑策略
    // 设置TableView控件使用数据模型
    ui->tableView->setModel(sd8200model);
    CustomItemDelegate8825 *delegate = new CustomItemDelegate8825;//设置编辑状态
    ui->tableView->setItemDelegate(delegate);
    sd8200model->select(); // 从数据库中获取数据

    for (int row = 0; row <= 197; row++) {
        if (row % 2 != 0) {
            for (int col = 3; col < 12; col++) {
                QModelIndex index =  sd8200model->index(row, col);
                 sd8200model->setData(index, 0);
            }
        }
    }
    // 提交更改到数据库中
    if (! sd8200model->submitAll()) {
        qDebug() << "Failed to submit changes to database:" <<  sd8200model->lastError().text();
        QMessageBox::information(this,"加载错误",sd8200model->lastError().text());

    }

    // 获取表头视图
    QHeaderView *headerView = ui->tableView->horizontalHeader();
    headerView->resizeSection(0,35); // 第一列宽度为35像素
    headerView->resizeSection(1,98);
    headerView->resizeSection(2,67);
    headerView->resizeSection(3,70);
    headerView->resizeSection(4,70);
    headerView->resizeSection(5,70);
    headerView->resizeSection(6,70);
    headerView->resizeSection(7,70);
    headerView->resizeSection(8,87);
    headerView->resizeSection(9,87);
    headerView->resizeSection(10,87);
    headerView->resizeSection(11,57);
    headerView->resizeSection(12,79);
    headerView->resizeSection(13,58);
    headerView->resizeSection(14,0);
}

void RtcMessage::sd8200Modelselt()
{
    // 创建数据模型
    sd8200model = new QSqlTableModel(this);
    sd8200model->setTable("SD8200RTCreg");
    sd8200model->setEditStrategy(QSqlTableModel::OnFieldChange); // 设置编辑策略
    sd8200model->select(); // 从数据库中获取数据
    // 设置TableView控件使用数据模型
    ui->tableView->setModel(sd8200model);
}

//2010
void RtcMessage::sd2010()
{
    ui->rtctype->update();

    ui->R00H->setText("读寄存器00H-13H");
    ui->R20H->setText("读RAM寄存器");

    ui->W00H->setText("写寄存器00H-06H");
    ui->W07H->setText("写寄存器00H-13H");
    ui->W2CH->setText("写RAM寄存器");

    SD2010=new RTCdata;
    SD2010->SD2010CreateDB();
    sd2010model = new QSqlTableModel(this);
    sd2010model->setTable("SD2010RTCreg");
    sd2010model->setEditStrategy(QSqlTableModel::OnFieldChange); // 设置编辑策略
    // 设置TableView控件使用数据模型
    ui->tableView->setModel(sd2010model);
    CustomItemDelegate *delegate = new CustomItemDelegate;//设置可编辑区域
    ui->tableView->setItemDelegate(delegate);
    sd2010model->select(); // 从数据库中获取数据

    // 将前64行的奇数行、前8列的单元格设置为0
    for (int row = 0; row <= 63; row++) {
        if (row % 2 != 0) {
            for (int col = 3; col < 12; col++) {
                QModelIndex index =  sd2010model->index(row, col);
                 sd2010model->setData(index, 0);
            }
        }
    }

    // 提交更改到数据库中
    if (! sd2010model->submitAll()) {
        qDebug() << "Failed to submit changes to database:" <<  sd2010model->lastError().text();
        QMessageBox::information(this,"加载错误",sd2010model->lastError().text());
    }

    // 获取表头视图
    QHeaderView *headerView = ui->tableView->horizontalHeader();
    headerView->resizeSection(0,35); // 第一列宽度为35像素
    headerView->resizeSection(1,94);
    headerView->resizeSection(2,88);
    headerView->resizeSection(3,70);
    headerView->resizeSection(4,70);
    headerView->resizeSection(5,70);
    headerView->resizeSection(6,70);
    headerView->resizeSection(7,70);
    headerView->resizeSection(8,70);
    headerView->resizeSection(9,70);
    headerView->resizeSection(10,70);
    headerView->resizeSection(11,60);
    headerView->resizeSection(12,79);
    headerView->resizeSection(13,89);
    headerView->resizeSection(14,0);
}

void RtcMessage::sd2010Modelselt()
{
    // 创建数据模型
    sd2010model = new QSqlTableModel(this);
    sd2010model->setTable("SD2010RTCreg");
    sd2010model->setEditStrategy(QSqlTableModel::OnFieldChange); // 设置编辑策略
    sd2010model->select(); // 从数据库中获取数据
    // 设置TableView控件使用数据模型
    ui->tableView->setModel(sd2010model);
}

//2058
void RtcMessage::sd2058()
{
    ui->rtctype->update();
    ui->R00H->setText("读寄存器00H-13H");
    ui->R20H->setText("读RAM寄存器");

    ui->W00H->setText("写寄存器00H-06H");
    ui->W07H->setText("写寄存器00H-13H");
    ui->W2CH->setText("写RAM寄存器");

    SD2058=new RTCdata;
    SD2058->SD2058CreateDB();
    sd2058model = new QSqlTableModel(this);
    sd2058model->setTable("SD2058RTCreg");
    sd2058model->setEditStrategy(QSqlTableModel::OnFieldChange); // 设置编辑策略
    // 设置TableView控件使用数据模型
    ui->tableView->setModel(sd2058model);
    CustomItemDelegate8825 *delegate = new CustomItemDelegate8825;//设置可编辑区域
    ui->tableView->setItemDelegate(delegate);
    sd2058model->select(); // 从数据库中获取数据

    // 将前64行的奇数行、前8列的单元格设置为0
    for (int row = 0; row <= 127; row++) {
        if (row % 2 != 0) {
            for (int col = 3; col < 12; col++) {
                QModelIndex index =  sd2058model->index(row, col);
                 sd2058model->setData(index, 0);
            }
        }
    }
    // 提交更改到数据库中
    if (! sd2058model->submitAll()) {
        qDebug() << "Failed to submit changes to database:" <<  sd2058model->lastError().text();
        QMessageBox::information(this,"加载错误",sd2058model->lastError().text());
    }

    // 获取表头视图
    QHeaderView *headerView = ui->tableView->horizontalHeader();
    headerView->resizeSection(0,35); // 第一列宽度为35像素
    headerView->resizeSection(1,94);
    headerView->resizeSection(2,88);
    headerView->resizeSection(3,70);
    headerView->resizeSection(4,70);
    headerView->resizeSection(5,70);
    headerView->resizeSection(6,70);
    headerView->resizeSection(7,70);
    headerView->resizeSection(8,70);
    headerView->resizeSection(9,70);
    headerView->resizeSection(10,70);
    headerView->resizeSection(11,60);
    headerView->resizeSection(12,79);
    headerView->resizeSection(13,89);
    headerView->resizeSection(14,0);


}

void RtcMessage::sd2058Modelselt()
{
    // 创建数据模型
    sd2058model = new QSqlTableModel(this);
    sd2058model->setTable("SD2058RTCreg");
    sd2058model->setEditStrategy(QSqlTableModel::OnFieldChange); // 设置编辑策略
    sd2058model->select(); // 从数据库中获取数据
    // 设置TableView控件使用数据模型
    ui->tableView->setModel(sd2058model);
}

//2059
void RtcMessage::sd2059()
{
    ui->rtctype->update();
    ui->R00H->setText("读寄存器00H-13H");
    ui->R20H->setText("读RAM寄存器");

    ui->W00H->setText("写寄存器00H-06H");
    ui->W07H->setText("写寄存器00H-13H");
    ui->W2CH->setText("写RAM寄存器");

    SD2059=new RTCdata;
    SD2059->SD2059CreateDB();
    sd2059model = new QSqlTableModel(this);
    sd2059model->setTable("SD2059RTCreg");
    sd2059model->setEditStrategy(QSqlTableModel::OnFieldChange); // 设置编辑策略
    // 设置TableView控件使用数据模型
    ui->tableView->setModel(sd2059model);
    CustomItemDelegate8825 *delegate = new CustomItemDelegate8825;//设置可编辑区域
    ui->tableView->setItemDelegate(delegate);
    sd2059model->select(); // 从数据库中获取数据

    // 将前64行的奇数行、前8列的单元格设置为0
    for (int row = 0; row <= 127; row++) {
        if (row % 2 != 0) {
            for (int col = 3; col < 12; col++) {
                QModelIndex index =  sd2059model->index(row, col);
                 sd2059model->setData(index, 0);
            }
        }
    }
    // 提交更改到数据库中
    if (! sd2059model->submitAll()) {
        qDebug() << "Failed to submit changes to database:" <<  sd2059model->lastError().text();
        QMessageBox::information(this,"加载错误",sd2059model->lastError().text());
    }

    // 获取表头视图
    QHeaderView *headerView = ui->tableView->horizontalHeader();
    headerView->resizeSection(0,35); // 第一列宽度为35像素
    headerView->resizeSection(1,94);
    headerView->resizeSection(2,88);
    headerView->resizeSection(3,70);
    headerView->resizeSection(4,70);
    headerView->resizeSection(5,70);
    headerView->resizeSection(6,70);
    headerView->resizeSection(7,70);
    headerView->resizeSection(8,70);
    headerView->resizeSection(9,70);
    headerView->resizeSection(10,70);
    headerView->resizeSection(11,60);
    headerView->resizeSection(12,79);
    headerView->resizeSection(13,89);
    headerView->resizeSection(14,0);

}

void RtcMessage::sd2059Modelselt()
{
    // 创建数据模型
    sd2059model = new QSqlTableModel(this);
    sd2059model->setTable("SD2059RTCreg");
    sd2059model->setEditStrategy(QSqlTableModel::OnFieldChange); // 设置编辑策略
    sd2059model->select(); // 从数据库中获取数据
    // 设置TableView控件使用数据模型
    ui->tableView->setModel(sd2059model);
}

//2068
void RtcMessage::sd2068()
{
    ui->rtctype->update();

    ui->R00H->setText("读寄存器00H-13H");
    ui->R20H->setText("读RAM寄存器");

    ui->W00H->setText("写寄存器00H-06H");
    ui->W07H->setText("写寄存器00H-13H");
    ui->W2CH->setText("写RAM寄存器");

    SD2068=new RTCdata;
    SD2068->SD2068CreateDB();
    sd2068model = new QSqlTableModel(this);
    sd2068model->setTable("SD2068RTCreg");
    sd2068model->setEditStrategy(QSqlTableModel::OnFieldChange); // 设置编辑策略
    // 设置TableView控件使用数据模型
    ui->tableView->setModel(sd2068model);
    CustomItemDelegate8825 *delegate = new CustomItemDelegate8825;//设置可编辑区域
    ui->tableView->setItemDelegate(delegate);
    sd2068model->select(); // 从数据库中获取数据

    // 将前64行的奇数行、前8列的单元格设置为0
    for (int row = 0; row <= 127; row++) {
        if (row % 2 != 0) {
            for (int col = 3; col < 12; col++) {
                QModelIndex index =  sd2068model->index(row, col);
                 sd2068model->setData(index, 0);
            }
        }
    }
    // 提交更改到数据库中
    if (! sd2068model->submitAll()) {
        qDebug() << "Failed to submit changes to database:" <<  sd2068model->lastError().text();
        QMessageBox::information(this,"加载错误",sd2068model->lastError().text());
    }

    // 获取表头视图
    QHeaderView *headerView = ui->tableView->horizontalHeader();
    headerView->resizeSection(0,35); // 第一列宽度为35像素
    headerView->resizeSection(1,94);
    headerView->resizeSection(2,88);
    headerView->resizeSection(3,70);
    headerView->resizeSection(4,70);
    headerView->resizeSection(5,70);
    headerView->resizeSection(6,70);
    headerView->resizeSection(7,70);
    headerView->resizeSection(8,70);
    headerView->resizeSection(9,70);
    headerView->resizeSection(10,70);
    headerView->resizeSection(11,60);
    headerView->resizeSection(12,79);
    headerView->resizeSection(13,89);
    headerView->resizeSection(14,0);

}

void RtcMessage::sd2068Modelselt()
{
    // 创建数据模型
    sd2068model = new QSqlTableModel(this);
    sd2068model->setTable("SD2068RTCreg");
    sd2068model->setEditStrategy(QSqlTableModel::OnFieldChange); // 设置编辑策略
    sd2068model->select(); // 从数据库中获取数据
    // 设置TableView控件使用数据模型
    ui->tableView->setModel(sd2068model);
}

//2069
void RtcMessage::sd2069()
{
    ui->rtctype->update();

    ui->R00H->setText("读寄存器00H-13H");
    ui->R20H->setText("读RAM寄存器");

    ui->W00H->setText("写寄存器00H-06H");
    ui->W07H->setText("写寄存器00H-13H");
    ui->W2CH->setText("写RAM寄存器");

    SD2069=new RTCdata;
    SD2069->SD2069CreateDB();
    sd2069model = new QSqlTableModel(this);
    sd2069model->setTable("SD2069RTCreg");
    sd2069model->setEditStrategy(QSqlTableModel::OnFieldChange); // 设置编辑策略
    // 设置TableView控件使用数据模型
    ui->tableView->setModel(sd2069model);
    CustomItemDelegate8825 *delegate = new CustomItemDelegate8825;//设置可编辑区域
    ui->tableView->setItemDelegate(delegate);
    sd2069model->select(); // 从数据库中获取数据

    // 将前64行的奇数行、前8列的单元格设置为0
    for (int row = 0; row <= 127; row++) {
        if (row % 2 != 0) {
            for (int col = 3; col < 12; col++) {
                QModelIndex index =  sd2069model->index(row, col);
                 sd2069model->setData(index, 0);
            }
        }
    }
    // 提交更改到数据库中
    if (! sd2069model->submitAll()) {
        qDebug() << "Failed to submit changes to database:" <<  sd2069model->lastError().text();
        QMessageBox::information(this,"加载错误",sd2069model->lastError().text());
    }

    // 获取表头视图
    QHeaderView *headerView = ui->tableView->horizontalHeader();
    headerView->resizeSection(0,35); // 第一列宽度为35像素
    headerView->resizeSection(1,94);
    headerView->resizeSection(2,88);
    headerView->resizeSection(3,70);
    headerView->resizeSection(4,70);
    headerView->resizeSection(5,70);
    headerView->resizeSection(6,70);
    headerView->resizeSection(7,70);
    headerView->resizeSection(8,70);
    headerView->resizeSection(9,70);
    headerView->resizeSection(10,70);
    headerView->resizeSection(11,60);
    headerView->resizeSection(12,79);
    headerView->resizeSection(13,89);
    headerView->resizeSection(14,0);
}

void RtcMessage::sd2069Modelselt()
{
    // 创建数据模型
    sd2069model = new QSqlTableModel(this);
    sd2069model->setTable("SD2069RTCreg");
    sd2069model->setEditStrategy(QSqlTableModel::OnFieldChange); // 设置编辑策略
    sd2069model->select(); // 从数据库中获取数据
    // 设置TableView控件使用数据模型
    ui->tableView->setModel(sd2069model);
}

//2077
void RtcMessage::sd2077()
{
    ui->rtctype->update();

    ui->R00H->setText("读寄存器00H-13H");
    ui->R20H->setText("读RAM寄存器");

    ui->W00H->setText("写寄存器00H-06H");
    ui->W07H->setText("写寄存器00H-13H");
    ui->W2CH->setText("写RAM寄存器");

    SD2077=new RTCdata;
    SD2077->SD2077CreateDB();
    sd2077model = new QSqlTableModel(this);
    sd2077model->setTable("SD2077RTCreg");
    sd2077model->setEditStrategy(QSqlTableModel::OnFieldChange); // 设置编辑策略
    // 设置TableView控件使用数据模型
    ui->tableView->setModel(sd2077model);
    CustomItemDelegate8825 *delegate = new CustomItemDelegate8825;//设置可编辑区域
    ui->tableView->setItemDelegate(delegate);
    sd2077model->select(); // 从数据库中获取数据

    // 将前64行的奇数行、前8列的单元格设置为0
    for (int row = 0; row <= 127; row++) {
        if (row % 2 != 0) {
            for (int col = 3; col < 12; col++) {
                QModelIndex index =  sd2077model->index(row, col);
                 sd2077model->setData(index, 0);
            }
        }
    }
    // 提交更改到数据库中
    if (! sd2077model->submitAll()) {
        qDebug() << "Failed to submit changes to database:" <<  sd2077model->lastError().text();
        QMessageBox::information(this,"加载错误",sd2077model->lastError().text());
    }

    // 获取表头视图
    QHeaderView *headerView = ui->tableView->horizontalHeader();
    headerView->resizeSection(0,35); // 第一列宽度为35像素
    headerView->resizeSection(1,94);
    headerView->resizeSection(2,88);
    headerView->resizeSection(3,70);
    headerView->resizeSection(4,70);
    headerView->resizeSection(5,70);
    headerView->resizeSection(6,70);
    headerView->resizeSection(7,70);
    headerView->resizeSection(8,70);
    headerView->resizeSection(9,70);
    headerView->resizeSection(10,70);
    headerView->resizeSection(11,60);
    headerView->resizeSection(12,79);
    headerView->resizeSection(13,89);
    headerView->resizeSection(14,0);
}

void RtcMessage::sd2077Modelselt()
{
    // 创建数据模型
    sd2077model = new QSqlTableModel(this);
    sd2077model->setTable("SD2077RTCreg");
    sd2077model->setEditStrategy(QSqlTableModel::OnFieldChange); // 设置编辑策略
    sd2077model->select(); // 从数据库中获取数据
    // 设置TableView控件使用数据模型
    ui->tableView->setModel(sd2077model);
}

//2078
void RtcMessage::sd2078()
{
    ui->rtctype->update();

    ui->R00H->setText("读寄存器00H-13H");
    ui->R20H->setText("读RAM寄存器");

    ui->W00H->setText("写寄存器00H-06H");
    ui->W07H->setText("写寄存器00H-13H");
    ui->W2CH->setText("写RAM寄存器");

    SD2078=new RTCdata;
    SD2078->SD2078CreateDB();
    sd2078model = new QSqlTableModel(this);
    sd2078model->setTable("SD2078RTCreg");
    sd2078model->setEditStrategy(QSqlTableModel::OnFieldChange); // 设置编辑策略
    // 设置TableView控件使用数据模型
    ui->tableView->setModel(sd2078model);
    CustomItemDelegate8825 *delegate = new CustomItemDelegate8825;//设置可编辑区域
    ui->tableView->setItemDelegate(delegate);
    sd2078model->select(); // 从数据库中获取数据

    // 将前64行的奇数行、前8列的单元格设置为0
    for (int row = 0; row <= 127; row++) {
        if (row % 2 != 0) {
            for (int col = 3; col < 12; col++) {
                QModelIndex index =  sd2078model->index(row, col);
                 sd2078model->setData(index, 0);
            }
        }
    }
    // 提交更改到数据库中
    if (! sd2078model->submitAll()) {
        qDebug() << "Failed to submit changes to database:" <<  sd2078model->lastError().text();
        QMessageBox::information(this,"加载错误",sd2078model->lastError().text());
    }

    // 获取表头视图
    QHeaderView *headerView = ui->tableView->horizontalHeader();
    headerView->resizeSection(0,35); // 第一列宽度为35像素
    headerView->resizeSection(1,94);
    headerView->resizeSection(2,88);
    headerView->resizeSection(3,70);
    headerView->resizeSection(4,70);
    headerView->resizeSection(5,70);
    headerView->resizeSection(6,70);
    headerView->resizeSection(7,70);
    headerView->resizeSection(8,70);
    headerView->resizeSection(9,70);
    headerView->resizeSection(10,70);
    headerView->resizeSection(11,60);
    headerView->resizeSection(12,79);
    headerView->resizeSection(13,89);
    headerView->resizeSection(14,0);
}

void RtcMessage::sd2078Modelselt()
{
    // 创建数据模型
    sd2078model = new QSqlTableModel(this);
    sd2078model->setTable("SD2078RTCreg");
    sd2078model->setEditStrategy(QSqlTableModel::OnFieldChange); // 设置编辑策略
    sd2078model->select(); // 从数据库中获取数据
    // 设置TableView控件使用数据模型
    ui->tableView->setModel(sd2078model);
}


//2200
void RtcMessage::sd2200()
{
    ui->rtctype->update();

    ui->R00H->setText("读所有寄存器");

    ui->W00H->setText("写时间1");
    ui->W07H->setText("写所有寄存器");

    SD2200=new RTCdata;
    SD2200->SD2200CreateDB();
    sd2200model = new QSqlTableModel(this);
    sd2200model->setTable("SD2200RTCreg");
    sd2200model->setEditStrategy(QSqlTableModel::OnFieldChange); // 设置编辑策略
    // 设置TableView控件使用数据模型
    ui->tableView->setModel(sd2200model);
    CustomItemDelegate8825 *delegate = new CustomItemDelegate8825;//设置可编辑区域
    ui->tableView->setItemDelegate(delegate);
    sd2200model->select(); // 从数据库中获取数据

    // 将前64行的奇数行、前8列的单元格设置为0
    for (int row = 0; row <= 127; row++) {
        if (row % 2 != 0) {
            for (int col = 3; col < 12; col++) {
                QModelIndex index =  sd2200model->index(row, col);
                 sd2200model->setData(index, 0);
            }
        }
    }
    // 提交更改到数据库中
    if (! sd2200model->submitAll()) {
        qDebug() << "Failed to submit changes to database:" <<  sd2200model->lastError().text();
        QMessageBox::information(this,"加载错误",sd2200model->lastError().text());
    }

    // 获取表头视图
    QHeaderView *headerView = ui->tableView->horizontalHeader();
    headerView->resizeSection(0,35); // 第一列宽度为35像素
    headerView->resizeSection(1,94);
    headerView->resizeSection(2,94);
    headerView->resizeSection(3,70);
    headerView->resizeSection(4,70);
    headerView->resizeSection(5,70);
    headerView->resizeSection(6,70);
    headerView->resizeSection(7,70);
    headerView->resizeSection(8,70);
    headerView->resizeSection(9,70);
    headerView->resizeSection(10,70);
    headerView->resizeSection(11,60);
    headerView->resizeSection(12,79);
    headerView->resizeSection(13,83);
    headerView->resizeSection(14,0);
}

void RtcMessage::sd2200Modelselt()
{
    // 创建数据模型
    sd2200model = new QSqlTableModel(this);
    sd2200model->setTable("SD2200RTCreg");
    sd2200model->setEditStrategy(QSqlTableModel::OnFieldChange); // 设置编辑策略
    sd2200model->select(); // 从数据库中获取数据
    // 设置TableView控件使用数据模型
    ui->tableView->setModel(sd2200model);

}


/**
功能：刷新数据--暂时未使用
修改：sjm
参数：-
返回值：-
**/
void RtcMessage::on_refresh_clicked()
{

}


/**
功能：保存数据--暂时未使用
修改：sjm
参数：-
返回值：-
**/
void RtcMessage::on_save_clicked()
{

}


/*********************************************************RTC读写寄存器槽函数实现***************************************************************/
/**
读写寄存器按键分布：

         |    温度值         电池
         |  ---------    ---------
         |  |  读1   |   | 写1-1  |
         |  ---------    ---------
         |  |  读2   |   | 写2-2  |
         |  ---------    ---------
         |  ---------    ---------
         |  |  读3   |   | 写3-3  |
         |  ---------    ---------
         |  ---------    ---------
         |  |  读4   |   | 写4-4  |
         |  ---------    ---------
         |  ---------    ---------
         |  |  读5   |   | 写5-5  |
         |  ---------    ---------
         |  ---------    ---------
         |  |  读6   |   | 写6-6  |
         |  ---------    ---------
         |  ---------    ---------
         |  |  读7   |   |校准时钟源|
         |  ---------    ---------

**/

/**
功能：校准时钟源按键--校准标准模块的时钟源
修改：sjm
参数：-
返回值：-
**/
void RtcMessage::on_CCS_clicked()
{
    //串口打开
    if(serialPort->isOpen())
    {
        // 获取当前日期和时间
        QDateTime currentDateTime = QDateTime::currentDateTime();

        // 拆分日期和时间
        int year = currentDateTime.date().year();
        int month = currentDateTime.date().month();
        int day = currentDateTime.date().day();
        int hour = currentDateTime.time().hour();
        int minute = currentDateTime.time().minute();
        int second = currentDateTime.time().second();
        int weekday = currentDateTime.date().dayOfWeek();

        // 将时间字段转换为BCD码并发送到单片机
        QByteArray bcdData;

        bcdData.append(static_cast<char>(((second / 10) << 4) | (second % 10)));  // 秒
        bcdData.append(static_cast<char>(((minute / 10) << 4) | (minute % 10)));  // 分钟
        bcdData.append(static_cast<char>(((hour / 10) << 4) | (hour % 10))|0x80);      // 小时
        bcdData.append(static_cast<char>(weekday)); // 星期
        bcdData.append(static_cast<char>(((day / 10) << 4) | (day % 10)));        // 日
        bcdData.append(static_cast<char>(((month / 10) << 4) | (month % 10)));    // 月
        bcdData.append(static_cast<char>(((year % 100 / 10) << 4) | (year % 100 % 10)));  // 年的后两位
        //发送给单片机--数据格式：BC CD XX
        QByteArray hexData;
        hexData.append(0xBC);
        hexData.append(0xCD);
        hexData.append(0xA8);
        serialPort->write(hexData);
        serialPort->write(bcdData);

        QMessageBox::information(this, "校准时钟源", "时钟源校准成功！");

    }else{

        QMessageBox::information(this,"错误","串口打开失败，串口未连接或者被占用!");

    }


}

/*********************************************************读寄存器槽函数实现***************************************************************/
/**
功能：读1按键--解析并显示时间和日期
修改：sjm
参数：-
返回值：-
**/
void RtcMessage::on_readtime_clicked()
{
    //获取选择的型号文本
    QString text = ui->rtctype->toPlainText();
    //清空输入缓冲区
    serialPort->clear(QSerialPort::Input);
    //打开串口
    if(serialPort->isOpen())
    {
        //选项的型号
        if(text=="SD3068"||text=="SD3010"||text=="SD3025"||text=="SD3031"||text=="SD3077"||text=="SD3078"||text=="SD3178"||text=="SD3900"||text=="SD2503"||text=="SD2505"||text=="SD2506"||text=="SD2507"||text=="SD2508"||text=="SD2520"||text=="SD2522"||text=="SD2523")
        {
            qDebug()<<"读串口名称:"<< serialPort->portName();
            qDebug() << " 读取时间";
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0xBF);

//            QByteArray hexData;
//            hexData.append(0xBC);
//            hexData.append(0xCD);
//            uint16_t value = 1;
//            QByteArray valueBytes(reinterpret_cast<const char*>(&value), sizeof(value));
//            hexData.append(valueBytes);

            //串口发送数据
            serialPort->write(hexData);
            //100ms后触发定时器
            QTimer::singleShot(100, [&](){
                QByteArray readtime=serialPort->readAll();//接收串口的数据
                qDebug()<<"收到数据："<<readtime;
                qDebug() <<readtime.size();

                if (!readtime.isEmpty()) {
                       // 解析BCD码时间
                       if (readtime.size() == 8) {
                           readtime[0] = readtime[0] & 0x7F;
                           uint8_t second = ((readtime[0] & 0xF0) >> 4) * 10 + (readtime[0] & 0x0F);
                           uint8_t minute = ((readtime[1] & 0xF0) >> 4) * 10 + (readtime[1] & 0x0F);
                           uint8_t hour = ((readtime[2] & 0x0F) + ((readtime[2] & 0x30) >> 4) * 10);
                           hour = hour & 0x7F; // 屏蔽最高位，将其置为0
                           uint8_t weekday = readtime[3];
                           uint8_t day = ((readtime[4] & 0xF0) >> 4) * 10 + (readtime[4] & 0x0F);
                           uint8_t month = ((readtime[5] & 0xF0) >> 4) * 10 + (readtime[5] & 0x0F);
                           uint8_t year = ((readtime[6] & 0xF0) >> 4) * 10 + (readtime[6] & 0x0F);

                           // 转换为十进制时间
                           QString decimalTime = QString("%1年%2月%3日 星期:%7  %4:%5:%6 ")
                                                     .arg(year)
                                                     .arg(month)
                                                     .arg(day)
                                                     .arg(hour,2,10,QChar('0'))
                                                     .arg(minute,2,10,QChar('0'))
                                                     .arg(second,2,10,QChar('0'))
                                                     .arg(chinese_week[weekday]);
                               // 在消息窗口显示消息信息
                               QMessageBox::information(this, "时间和日期", decimalTime);
                               qDebug() << "解析后时间：" << decimalTime;
                     }
                }
            });
        }else if (text=="SD8563"||text=="SD8564"||text=="SD8565"||text=="SD8568") {
            qDebug()<<"读串口名称:"<< serialPort->portName();
            qDebug() << " 读取时间";
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x10);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray readtime=serialPort->readAll();
                qDebug()<<"收到数据："<<readtime;
                qDebug() <<readtime.size();

                if (!readtime.isEmpty()) {
                       // 解析BCD码时间
                       if (readtime.size() == 8) {
                           readtime[0]=readtime[0]&0x7F;
                           uint8_t second = ((readtime[0] & 0xF0) >> 4) * 10 + (readtime[0] & 0x0F);
                           uint8_t minute = ((readtime[1] & 0xF0) >> 4) * 10 + (readtime[1] & 0x0F);
                           uint8_t hour = ((readtime[2] & 0xF0) >> 4) * 10 + (readtime[2] & 0x0F);
                           uint8_t weekday = readtime[4];
                           uint8_t day = ((readtime[3] & 0xF0) >> 4) * 10 + (readtime[3] & 0x0F);
                           uint8_t month = ((readtime[5] & 0xF0) >> 4) * 10 + (readtime[5] & 0x0F);
                           uint8_t year = ((readtime[6] & 0xF0) >> 4) * 10 + (readtime[6] & 0x0F);

                           // 转换为十进制时间
                           QString decimalTime = QString("%1年%2月%3日 星期:%7  %4:%5:%6 ")
                                                     .arg(year)
                                                     .arg(month)
                                                     .arg(day)
                                                     .arg(hour,2,10,QChar('0'))
                                                     .arg(minute,2,10,QChar('0'))
                                                     .arg(second,2,10,QChar('0'))
                                                     .arg(chinese_week[weekday]);
                               // 在消息窗口显示消息信息
                               QMessageBox::information(this, "时间和日期", decimalTime);
                               qDebug() << "解析后时间：" << decimalTime;
                     }
                }
            });

        }else if (text=="SD8810") {
            qDebug()<<"读串口名称:"<< serialPort->portName();
            qDebug() << " 读取时间";
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x14);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray readtime=serialPort->readAll();
                qDebug()<<"收到数据："<<readtime;
                qDebug() <<readtime.size();

                if (!readtime.isEmpty()) {
                       // 解析BCD码时间
                       if (readtime.size() == 8) {
                           uint8_t second = ((readtime[0] & 0xF0) >> 4) * 10 + (readtime[0] & 0x0F);
                           uint8_t minute = ((readtime[1] & 0xF0) >> 4) * 10 + (readtime[1] & 0x0F);
                           uint8_t hour = ((readtime[2] & 0x0F) + ((readtime[2] & 0x30) >> 4) * 10);
                           uint8_t weekday = readtime[3];
                           uint8_t day = ((readtime[4] & 0xF0) >> 4) * 10 + (readtime[4] & 0x0F);
                           uint8_t month = ((readtime[5] & 0xF0) >> 4) * 10 + (readtime[5] & 0x0F);
                           uint8_t year = ((readtime[6] & 0xF0) >> 4) * 10 + (readtime[6] & 0x0F);

                           // 转换为十进制时间
                           QString decimalTime = QString("%1年%2月%3日 星期:%7  %4:%5:%6 ")
                                                    .arg(year)
                                                    .arg(month)
                                                    .arg(day)
                                                    .arg(hour,2,10,QChar('0'))
                                                    .arg(minute,2,10,QChar('0'))
                                                    .arg(second,2,10,QChar('0'))
                                                    .arg(chinese_week[weekday]);
                               // 在消息窗口显示消息信息
                               QMessageBox::information(this, "时间和日期", decimalTime);
                               qDebug() << "解析后时间：" << decimalTime;
                     }
                }
            });

        }else if (text=="SD8939") {
            qDebug()<<"读串口名称:"<< serialPort->portName();
            qDebug() << " 读取时间";
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x26);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray readtime=serialPort->readAll();
                qDebug()<<"收到数据："<<readtime;
                qDebug() <<readtime.size();

                if (!readtime.isEmpty()) {
                       // 解析BCD码时间
                       if (readtime.size() == 8) {
                           uint8_t second = ((readtime[0] & 0xF0) >> 4) * 10 + (readtime[0] & 0x0F);
                           uint8_t minute = ((readtime[1] & 0xF0) >> 4) * 10 + (readtime[1] & 0x0F);
                           uint8_t hour = ((readtime[2] & 0x0F) + ((readtime[2] & 0x30) >> 4) * 10);
                           uint8_t weekday = readtime[3];
                           uint8_t day = ((readtime[4] & 0xF0) >> 4) * 10 + (readtime[4] & 0x0F);
                           uint8_t month = ((readtime[5] & 0xF0) >> 4) * 10 + (readtime[5] & 0x0F);
                           uint8_t year = ((readtime[6] & 0xF0) >> 4) * 10 + (readtime[6] & 0x0F);

                           // 转换为十进制时间
                           QString decimalTime = QString("%1年%2月%3日 星期:%7  %4:%5:%6 ")
                                                     .arg(year)
                                                     .arg(month)
                                                     .arg(day)
                                                     .arg(hour,2,10,QChar('0'))
                                                     .arg(minute,2,10,QChar('0'))
                                                     .arg(second,2,10,QChar('0'))
                                                     .arg(chinese_week[weekday]);
                               // 在消息窗口显示消息信息
                               QMessageBox::information(this, "时间和日期", decimalTime);
                               qDebug() << "解析后时间：" << decimalTime;
                     }
                }
            });

        }else if (text=="SD8800") {
            qDebug()<<"读串口名称:"<< serialPort->portName();
            qDebug() << " 读取时间";
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x36);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray readtime=serialPort->readAll();
                qDebug()<<"收到数据："<<readtime;
                qDebug() <<readtime.size();

                if (!readtime.isEmpty()) {
                       // 解析BCD码时间
                       if (readtime.size() == 8) {
                           uint8_t second = ((readtime[0] & 0xF0) >> 4) * 10 + (readtime[0] & 0x0F);
                           uint8_t minute = ((readtime[1] & 0xF0) >> 4) * 10 + (readtime[1] & 0x0F);
                           uint8_t hour = ((readtime[2] & 0x0F) + ((readtime[2] & 0x30) >> 4) * 10);
                           uint8_t weekday = readtime[3];
                           uint8_t day = ((readtime[4] & 0xF0) >> 4) * 10 + (readtime[4] & 0x0F);
                           uint8_t month = ((readtime[5] & 0xF0) >> 4) * 10 + (readtime[5] & 0x0F);
                           uint8_t year = ((readtime[6] & 0xF0) >> 4) * 10 + (readtime[6] & 0x0F);

                           // 转换为十进制时间
                           QString decimalTime = QString("%1年%2月%3日 星期:%7  %4:%5:%6 ")
                                                     .arg(year)
                                                     .arg(month)
                                                     .arg(day)
                                                     .arg(hour,2,10,QChar('0'))
                                                     .arg(minute,2,10,QChar('0'))
                                                     .arg(second,2,10,QChar('0'))
                                                     .arg(chinese_week[weekday]);
                               // 在消息窗口显示消息信息
                               QMessageBox::information(this, "时间和日期", decimalTime);
                               qDebug() << "解析后时间：" << decimalTime;
                     }
                }
            });

        }else if (text=="SD8931") {
            qDebug()<<"读串口名称:"<< serialPort->portName();
            qDebug() << " 读取时间";
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x48);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray readtime=serialPort->readAll();
                qDebug()<<"收到数据："<<readtime;
                qDebug() <<readtime.size();

                if (!readtime.isEmpty()) {
                       // 解析BCD码时间
                       if (readtime.size() == 8) {
                           uint8_t second = ((readtime[0] & 0xF0) >> 4) * 10 + (readtime[0] & 0x0F);
                           uint8_t minute = ((readtime[1] & 0xF0) >> 4) * 10 + (readtime[1] & 0x0F);
                           uint8_t hour = ((readtime[2] & 0x0F) + ((readtime[2] & 0x30) >> 4) * 10);
                           uint8_t weekday = readtime[3];
                           uint8_t day = ((readtime[4] & 0xF0) >> 4) * 10 + (readtime[4] & 0x0F);
                           uint8_t month = ((readtime[5] & 0xF0) >> 4) * 10 + (readtime[5] & 0x0F);
                           uint8_t year = ((readtime[6] & 0xF0) >> 4) * 10 + (readtime[6] & 0x0F);

                           // 转换为十进制时间
                           QString decimalTime = QString("%1年%2月%3日 星期:%7  %4:%5:%6 ")
                                                    .arg(year)
                                                    .arg(month)
                                                    .arg(day)
                                                    .arg(hour,2,10,QChar('0'))
                                                    .arg(minute,2,10,QChar('0'))
                                                    .arg(second,2,10,QChar('0'))
                                                    .arg(chinese_week[weekday]);
                               // 在消息窗口显示消息信息
                               QMessageBox::information(this, "时间和日期", decimalTime);
                               qDebug() << "解析后时间：" << decimalTime;
                     }
                }
            });

        }else if (text=="SD8825") {
            qDebug()<<"读串口名称:"<< serialPort->portName();
            qDebug() << " 读取时间";
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x60);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray readtime=serialPort->readAll();
                qDebug()<<"收到数据："<<readtime;
                qDebug() <<readtime.size();

                if (!readtime.isEmpty()) {
                       // 解析BCD码时间
                       if (readtime.size() == 8) {
                           uint8_t second = ((readtime[0] & 0xF0) >> 4) * 10 + (readtime[0] & 0x0F);
                           uint8_t minute = ((readtime[1] & 0xF0) >> 4) * 10 + (readtime[1] & 0x0F);
                           uint8_t hour = ((readtime[2] & 0x0F) + ((readtime[2] & 0x30) >> 4) * 10);
                           uint8_t weekday = readtime[3];
                           uint8_t day = ((readtime[4] & 0xF0) >> 4) * 10 + (readtime[4] & 0x0F);
                           uint8_t month = ((readtime[5] & 0xF0) >> 4) * 10 + (readtime[5] & 0x0F);
                           uint8_t year = ((readtime[6] & 0xF0) >> 4) * 10 + (readtime[6] & 0x0F);

                           // 转换为十进制时间
                           QString decimalTime = QString("%1年%2月%3日 星期:%7  %4:%5:%6 ")
                                                    .arg(year)
                                                    .arg(month)
                                                    .arg(day)
                                                    .arg(hour,2,10,QChar('0'))
                                                    .arg(minute,2,10,QChar('0'))
                                                    .arg(second,2,10,QChar('0'))
                                                    .arg(chinese_week[weekday]);
                               // 在消息窗口显示消息信息
                               QMessageBox::information(this, "时间和日期", decimalTime);
                               qDebug() << "解析后时间：" << decimalTime;
                     }
                }
            });

        }else if (text=="SD8200") {
            qDebug()<<"读串口名称:"<< serialPort->portName();
            qDebug() << " 读取时间";
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x70);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray readtime=serialPort->readAll();
                qDebug()<<"收到数据："<<readtime;
                qDebug() <<readtime.size();

                if (!readtime.isEmpty()) {
                       // 解析BCD码时间
                       if (readtime.size() == 8) {
                           uint8_t second = ((readtime[0] & 0xF0) >> 4) * 10 + (readtime[0] & 0x0F);
                           uint8_t minute = ((readtime[1] & 0xF0) >> 4) * 10 + (readtime[1] & 0x0F);
                           uint8_t hour = ((readtime[2] & 0x0F) + ((readtime[2] & 0x30) >> 4) * 10);
                           uint8_t weekday = readtime[3];
                           uint8_t day = ((readtime[4] & 0xF0) >> 4) * 10 + (readtime[4] & 0x0F);
                           uint8_t month = ((readtime[5] & 0xF0) >> 4) * 10 + (readtime[5] & 0x0F);
                           uint8_t year = ((readtime[6] & 0xF0) >> 4) * 10 + (readtime[6] & 0x0F);

                           // 转换为十进制时间
                           QString decimalTime = QString("%1年%2月%3日 星期:%7  %4:%5:%6 ")
                                                    .arg(year)
                                                    .arg(month)
                                                    .arg(day)
                                                    .arg(hour,2,10,QChar('0'))
                                                    .arg(minute,2,10,QChar('0'))
                                                    .arg(second,2,10,QChar('0'))
                                                    .arg(chinese_week[weekday]);
                               // 在消息窗口显示消息信息
                               QMessageBox::information(this, "时间和日期", decimalTime);
                               qDebug() << "解析后时间：" << decimalTime;
                     }
                }
            });

        }else if (text=="SD8804") {
            qDebug()<<"读串口名称:"<< serialPort->portName();
            qDebug() << " 读取时间";
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x3A);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray readtime=serialPort->readAll();
                qDebug()<<"收到数据："<<readtime;
                qDebug() <<readtime.size();

                if (!readtime.isEmpty()) {
                       // 解析BCD码时间
                       if (readtime.size() == 8) {
                           uint8_t second = ((readtime[0] & 0xF0) >> 4) * 10 + (readtime[0] & 0x0F);
                           uint8_t minute = ((readtime[1] & 0xF0) >> 4) * 10 + (readtime[1] & 0x0F);
                           uint8_t hour = ((readtime[2] & 0x0F) + ((readtime[2] & 0x30) >> 4) * 10);
                           uint8_t weekday = readtime[3];
                           uint8_t day = ((readtime[4] & 0xF0) >> 4) * 10 + (readtime[4] & 0x0F);
                           uint8_t month = ((readtime[5] & 0xF0) >> 4) * 10 + (readtime[5] & 0x0F);
                           uint8_t year = ((readtime[6] & 0xF0) >> 4) * 10 + (readtime[6] & 0x0F);

                           // 转换为十进制时间
                           QString decimalTime = QString("%1年%2月%3日 星期:%7  %4:%5:%6 ")
                                                     .arg(year)
                                                     .arg(month)
                                                     .arg(day)
                                                     .arg(hour,2,10,QChar('0'))
                                                     .arg(minute,2,10,QChar('0'))
                                                     .arg(second,2,10,QChar('0'))
                                                     .arg(chinese_week[weekday]);
                               // 在消息窗口显示消息信息
                               QMessageBox::information(this, "时间和日期", decimalTime);
                               qDebug() << "解析后时间：" << decimalTime;
                         }
                   }
            });

        }else if (text=="SD8840") {
            qDebug()<<"读串口名称:"<< serialPort->portName();
            qDebug() << " 读取时间";
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x90);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray readtime=serialPort->readAll();
                qDebug()<<"收到数据："<<readtime;
                qDebug() <<readtime.size();

                if (!readtime.isEmpty()) {
                       // 解析BCD码时间
                       if (readtime.size() == 8) {
                           uint8_t second = ((readtime[0] & 0xF0) >> 4) * 10 + (readtime[0] & 0x0F);
                           uint8_t minute = ((readtime[1] & 0xF0) >> 4) * 10 + (readtime[1] & 0x0F);
                           uint8_t hour = ((readtime[2] & 0x0F) + ((readtime[2] & 0x30) >> 4) * 10);
                           uint8_t weekday = readtime[3];
                           uint8_t day = ((readtime[4] & 0xF0) >> 4) * 10 + (readtime[4] & 0x0F);
                           uint8_t month = ((readtime[5] & 0xF0) >> 4) * 10 + (readtime[5] & 0x0F);
                           uint8_t year = ((readtime[6] & 0xF0) >> 4) * 10 + (readtime[6] & 0x0F);

                           // 转换为十进制时间
                           QString decimalTime = QString("%1年%2月%3日 星期:%7  %4:%5:%6 ")
                                   .arg(year)
                                   .arg(month)
                                   .arg(day)
                                   .arg(hour,2,10,QChar('0'))
                                   .arg(minute,2,10,QChar('0'))
                                   .arg(second,2,10,QChar('0'))
                                   .arg(chinese_week[weekday]);
                               // 在消息窗口显示消息信息
                               QMessageBox::information(this, "时间和日期", decimalTime);
                               qDebug() << "解析后时间：" << decimalTime;
                         }
                   }
            });

        }else if (text=="SD8902") {
            qDebug()<<"读串口名称:"<< serialPort->portName();
            qDebug() << " 读取时间";
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x98);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray readtime=serialPort->readAll();
                qDebug()<<"收到数据："<<readtime;
                qDebug() <<readtime.size();

                if (!readtime.isEmpty()) {
                       // 解析BCD码时间
                       if (readtime.size() == 8) {
                           uint8_t second = ((readtime[0] & 0xF0) >> 4) * 10 + (readtime[0] & 0x0F);
                           uint8_t minute = ((readtime[1] & 0xF0) >> 4) * 10 + (readtime[1] & 0x0F);
                           uint8_t hour = ((readtime[2] & 0x0F) + ((readtime[2] & 0x30) >> 4) * 10);
                           uint8_t day = ((readtime[3] & 0xF0) >> 4) * 10 + (readtime[3] & 0x0F);
                           uint8_t month = ((readtime[4] & 0xF0) >> 4) * 10 + (readtime[4] & 0x0F);
                           uint8_t weekday = readtime[5];
                           uint8_t year = ((readtime[6] & 0xF0) >> 4) * 10 + (readtime[6] & 0x0F);

                           // 转换为十进制时间
                           QString decimalTime = QString("%1年%2月%3日 星期:%7  %4:%5:%6 ")
                                                        .arg(year)
                                                        .arg(month)
                                                        .arg(day)
                                                        .arg(hour,2,10,QChar('0'))
                                                        .arg(minute,2,10,QChar('0'))
                                                        .arg(second,2,10,QChar('0'))
                                                        .arg(chinese_week[weekday]);
                               // 在消息窗口显示消息信息
                               QMessageBox::information(this, "时间和日期", decimalTime);
                               qDebug() << "解析后时间：" << decimalTime;
                         }
                   }
            });
        }else if (text=="SD2010") {
            qDebug()<<"读串口名称:"<< serialPort->portName();
            qDebug() << " 读取时间";
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            uint16_t value = 1;
            QByteArray valueBytes(reinterpret_cast<const char*>(&value), sizeof(value));
            hexData.append(valueBytes);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray readtime=serialPort->readAll();
                qDebug()<<"收到数据："<<readtime;
                qDebug() <<readtime.size();

                if (!readtime.isEmpty()) {
                       // 解析BCD码时间
                       if (readtime.size() == 8) {
                           uint8_t second = ((readtime[0] & 0xF0) >> 4) * 10 + (readtime[0] & 0x0F);
                           uint8_t minute = ((readtime[1] & 0xF0) >> 4) * 10 + (readtime[1] & 0x0F);
                           uint8_t hour = ((readtime[2] & 0x0F) + ((readtime[2] & 0x30) >> 4) * 10);
                           hour = hour & 0x7F; // 屏蔽最高位，将其置为0
                           uint8_t weekday = readtime[3];
                           uint8_t day = ((readtime[4] & 0xF0) >> 4) * 10 + (readtime[4] & 0x0F);
                           uint8_t month = ((readtime[5] & 0xF0) >> 4) * 10 + (readtime[5] & 0x0F);
                           uint8_t year = ((readtime[6] & 0xF0) >> 4) * 10 + (readtime[6] & 0x0F);

                           // 转换为十进制时间
                           QString decimalTime = QString("%1年%2月%3日 星期:%7  %4:%5:%6 ")
                                                     .arg(year)
                                                     .arg(month)
                                                     .arg(day)
                                                     .arg(hour,2,10,QChar('0'))
                                                     .arg(minute,2,10,QChar('0'))
                                                     .arg(second,2,10,QChar('0'))
                                                     .arg(chinese_week[weekday]);
                               // 在消息窗口显示消息信息
                               QMessageBox::information(this, "时间和日期", decimalTime);
                               qDebug() << "解析后时间：" << decimalTime;
                     }
                }
            });
        }else if (text=="SD2058"||text=="SD2059"||text=="SD2068"||text=="SD2069"||text=="SD2077"||text=="SD2078"||text=="SD2400"||text=="SD2401"||text=="SD2402"||text=="SD2403"||text=="SD2404"||text=="SD2405") {
            qDebug()<<"读串口名称:"<< serialPort->portName();
            qDebug() << " 读取时间";
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0xBF);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray readtime=serialPort->readAll();
                qDebug()<<"收到数据："<<readtime;
                qDebug() <<readtime.size();

                if (!readtime.isEmpty()) {
                       // 解析BCD码时间
                       if (readtime.size() == 8) {
                           uint8_t second = ((readtime[0] & 0xF0) >> 4) * 10 + (readtime[0] & 0x0F);
                           uint8_t minute = ((readtime[1] & 0xF0) >> 4) * 10 + (readtime[1] & 0x0F);
                           uint8_t hour = ((readtime[2] & 0x0F) + ((readtime[2] & 0x30) >> 4) * 10);
                           hour = hour & 0x7F; // 屏蔽最高位，将其置为0
                           uint8_t weekday = readtime[3];
                           uint8_t day = ((readtime[4] & 0xF0) >> 4) * 10 + (readtime[4] & 0x0F);
                           uint8_t month = ((readtime[5] & 0xF0) >> 4) * 10 + (readtime[5] & 0x0F);
                           uint8_t year = ((readtime[6] & 0xF0) >> 4) * 10 + (readtime[6] & 0x0F);

                           // 转换为十进制时间
                           QString decimalTime = QString("%1年%2月%3日 星期:%7  %4:%5:%6 ")
                                                     .arg(year)
                                                     .arg(month)
                                                     .arg(day)
                                                     .arg(hour,2,10,QChar('0'))
                                                     .arg(minute,2,10,QChar('0'))
                                                     .arg(second,2,10,QChar('0'))
                                                     .arg(chinese_week[weekday]);
                               // 在消息窗口显示消息信息
                               QMessageBox::information(this, "时间和日期", decimalTime);
                               qDebug() << "解析后时间：" << decimalTime;
                     }
                }
            });
        }else if (text=="SD2200"||text=="SD2201"||text=="SD2202"||text=="SD2203"||text=="SD2204")
        {
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            uint16_t value = 8;
            QByteArray valueBytes(reinterpret_cast<const char*>(&value), sizeof(value));
            hexData.append(valueBytes);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray readtime=serialPort->readAll();
                qDebug()<<"收到数据："<<readtime;
                qDebug() <<readtime.size();

                if (!readtime.isEmpty()) {
                       // 解析BCD码时间
                       if (readtime.size() == 7) {
                           uint8_t second = ((readtime[6] & 0xF0) >> 4) * 10 + (readtime[6] & 0x0F);
                           uint8_t minute = ((readtime[5] & 0xF0) >> 4) * 10 + (readtime[5] & 0x0F);
                           uint8_t hour = ((readtime[4] & 0x0F) + ((readtime[4] & 0x30) >> 4) * 10);
                           uint8_t weekday = readtime[3];
                           uint8_t day = ((readtime[2] & 0xF0) >> 4) * 10 + (readtime[2] & 0x0F);
                           uint8_t month = ((readtime[1] & 0xF0) >> 4) * 10 + (readtime[1] & 0x0F);
                           uint8_t year = ((readtime[0] & 0xF0) >> 4) * 10 + (readtime[0] & 0x0F);

                           // 转换为十进制时间
                           QString decimalTime = QString("%1年%2月%3日 星期:%7  %4:%5:%6 ")
                                                     .arg(year)
                                                     .arg(month)
                                                     .arg(day)
                                                     .arg(hour,2,10,QChar('0'))
                                                     .arg(minute,2,10,QChar('0'))
                                                     .arg(second,2,10,QChar('0'))
                                                     .arg(chinese_week[weekday]);
                               // 在消息窗口显示消息信息
                               QMessageBox::information(this, "时间和日期", decimalTime);
                               qDebug() << "解析后时间：" << decimalTime;
                     }
                }
            });
        }
        }else{
            QMessageBox::information(this,"错误","串口打开失败，串口未连接或者被占用!");
        }
}

/**
功能：读2按键--读取芯片的寄存器，具体寄存器根据具体芯片型号来确定
修改：sjm
参数：-
返回值：-
**/
void RtcMessage::on_R00H_clicked()
{
    QString text = ui->rtctype->toPlainText();
    //清空输入缓冲区
    serialPort->clear(QSerialPort::Input);

    if(serialPort->isOpen())
    {
        if(text=="SD3068"||text=="SD3010"||text=="SD3025"||text=="SD3031"||text=="SD3077"||text=="SD3078"||text=="SD3178"||text=="SD3900"||text=="SD2503"||text=="SD2505"||text=="SD2506"||text=="SD2507"||text=="SD2508"||text=="SD2520"||text=="SD2522"||text=="SD2523")
        {
            // 显示数据库模型
            sd3068Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0xA6);
            serialPort->write(hexData);
            QTimer::singleShot(100, [&](){
            QByteArray ROOdata=serialPort->readAll();
            qDebug()<<"收到数据："<<ROOdata;
            qDebug() <<ROOdata.size();

        //        int counter = 1;//行计数
        //        for (int i = 0; i < ROOdata.size(); i++) {
        //            QModelIndex index = sd3068model->index(counter, 8); // 第8列的索引
        //            QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
        //            QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
        //            sd3068model->setData(index, finalString);
        //            counter += 2; // 每隔一个单元格设置数据
        //        }

                    if (!ROOdata.isEmpty())
                    {
                       if(ROOdata.size()==33)//单片机发送33位数据
                       {
                                // 解析并设置BCD码数据
                               int row = 1;//行索引为1，数据库表中从0开始计数，意味着奇数行存放读取到的数据
                               int counter = 1;//行计数
                               int startCol = 3; // 定义数据存放的起始列
                               int endCol = 10; // 定义数据存放的结束列
                               int currentCol = startCol; // 当前处理的列从起始列开始
                               for (int i = 0; i < ROOdata.size() && i < 32; i++)//处理的数据不包括最后一位，最后一位是单片机发送的结束符--这样做有个好处，不用管单片机发送过来的数据是否有重复的
                               {
                                   QModelIndex index = sd3068model->index(counter, 11); // 第11列的索引
                                   QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();//转为16进制
                                   QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                                   sd3068model->setData(index, finalString);//在地11列显示读取到的16进制值
                                   counter += 2; // 每隔一个单元格设置数据
                                   //将数据转为二进制存放到对应的行和列中
                                   for (int j = 0; j < 8; j++) {
                                       bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                       QString binaryString = bit ? "1" : "0";
                                       sd3068model->setData(sd3068model->index(row, currentCol), binaryString);
                                       currentCol++;
                                       if (currentCol > endCol) {
                                           currentCol = startCol;
                                           row+=2;//隔一行存放数据
                                       }
                                   }
                               }

                               QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                               qDebug() << "读00H："<<ROOdata.size();
                       }

                    }
            });
        }else if (text=="SD8563"||text=="SD8564"||text=="SD8565"||text=="SD8568") {
            sd8563Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x12);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                    if(ROOdata.size()==17)
                  {
                        // 解析并设置BCD码数据
                        int row = 1;
                        int counter = 1;//行计数
                        int startCol = 3; // 定义数据存放的起始列
                        int endCol = 10; // 定义数据存放的结束列
                        int currentCol = startCol; // 当前处理的列从起始列开始
                        for (int i = 0; i < ROOdata.size() && i<16; i++) {
                            QModelIndex index = sd8563model->index(counter, 11); // 第11列的索引
                            QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                            QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                            sd8563model->setData(index, finalString);
                            counter += 2; // 每隔一个单元格设置数据

                            for (int j = 0; j < 8; j++) {
                                bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                QString binaryString = bit ? "1" : "0";
                                sd8563model->setData(sd8563model->index(row, currentCol), binaryString);
                                currentCol++;
                                if (currentCol > endCol) {
                                    currentCol = startCol;
                                    row+=2;//隔一行存放数据
                                }
                            }
                        }

                        QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                        qDebug() << "读00H："<<ROOdata.size();
                    }

                }
            });

        }else if (text=="SD8810") {
            sd8810Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x16);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                    if(ROOdata.size()==17)
                  {
                        // 解析并设置BCD码数据
                        int row = 1;
                        int counter = 1;//行计数
                        int startCol = 3; // 定义数据存放的起始列
                        int endCol = 10; // 定义数据存放的结束列
                        int currentCol = startCol; // 当前处理的列从起始列开始
                        for (int i = 0; i < ROOdata.size() && i<16; i++) {
                            QModelIndex index = sd8810model->index(counter, 11); // 第11列的索引
                            QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                            QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                            sd8810model->setData(index, finalString);
                            counter += 2; // 每隔一个单元格设置数据

                            for (int j = 0; j < 8; j++) {
                                bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                QString binaryString = bit ? "1" : "0";
                                sd8810model->setData(sd8810model->index(row, currentCol), binaryString);
                                currentCol++;
                                if (currentCol > endCol) {
                                    currentCol = startCol;
                                    row+=2;//隔一行存放数据
                                }
                            }
                        }
                        QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                        qDebug() << "读10H："<<ROOdata.size();
                    }
                }
            });

        }else if (text=="SD8939") {
            sd8939Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x28);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                    if(ROOdata.size()==21)
                  {
                        // 解析并设置BCD码数据
                        int row = 1;
                        int counter = 1;//行计数
                        int startCol = 3; // 定义数据存放的起始列
                        int endCol = 10; // 定义数据存放的结束列
                        int currentCol = startCol; // 当前处理的列从起始列开始
                        for (int i = 0; i < ROOdata.size() && i<20; i++) {
                            QModelIndex index = sd8939model->index(counter, 11); // 第11列的索引
                            QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                            QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                            sd8939model->setData(index, finalString);
                            counter += 2; // 每隔一个单元格设置数据

                            for (int j = 0; j < 8; j++) {
                                bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                QString binaryString = bit ? "1" : "0";
                                sd8939model->setData(sd8939model->index(row, currentCol), binaryString);
                                currentCol++;
                                if (currentCol > endCol) {
                                    currentCol = startCol;
                                    row+=2;//隔一行存放数据
                                }
                            }
                        }
                        QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                        qDebug() << "读10H："<<ROOdata.size();
                    }
                }
            });

        }else if (text=="SD8800") {
            sd8800Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x38);
            serialPort->write(hexData);

            QTimer::singleShot(110, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                    if(ROOdata.size()==17)
                  {
                        // 解析并设置BCD码数据
                        int row = 1;
                        int counter = 1;//行计数
                        int startCol = 3; // 定义数据存放的起始列
                        int endCol = 10; // 定义数据存放的结束列
                        int currentCol = startCol; // 当前处理的列从起始列开始
                        for (int i = 0; i < ROOdata.size() && i<16; i++) {
                            QModelIndex index = sd8800model->index(counter, 11); // 第11列的索引
                            QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                            QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                            sd8800model->setData(index, finalString);
                            counter += 2; // 每隔一个单元格设置数据

                            for (int j = 0; j < 8; j++) {
                                bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                QString binaryString = bit ? "1" : "0";
                                sd8800model->setData(sd8800model->index(row, currentCol), binaryString);
                                currentCol++;
                                if (currentCol > endCol) {
                                    currentCol = startCol;
                                    row+=2;//隔一行存放数据
                                }
                            }
                        }
                        QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                        qDebug() << "读10H："<<ROOdata.size();
                    }
                }
            });

        }else if (text=="SD8931") {
            sd8931Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x50);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                    if(ROOdata.size()==21)
                  {
                        // 解析并设置BCD码数据
                        int row = 1;
                        int counter = 1;//行计数
                        int startCol = 3; // 定义数据存放的起始列
                        int endCol = 10; // 定义数据存放的结束列
                        int currentCol = startCol; // 当前处理的列从起始列开始
                        for (int i = 0; i < ROOdata.size() && i<20; i++) {
                            QModelIndex index = sd8931model->index(counter, 11); // 第11列的索引
                            QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                            QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                            sd8931model->setData(index, finalString);
                            counter += 2; // 每隔一个单元格设置数据

                            for (int j = 0; j < 8; j++) {
                                bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                QString binaryString = bit ? "1" : "0";
                                sd8931model->setData(sd8931model->index(row, currentCol), binaryString);
                                currentCol++;
                                if (currentCol > endCol) {
                                    currentCol = startCol;
                                    row+=2;//隔一行存放数据
                                }
                            }
                        }
                        QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                        qDebug() << "读10H："<<ROOdata.size();
                    }
                }
            });

        }else if (text=="SD8825") {
            sd8825Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x62);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                    if(ROOdata.size()==21)
                  {
                        // 解析并设置BCD码数据
                        int row = 1;
                        int counter = 1;//行计数
                        int startCol = 3; // 定义数据存放的起始列
                        int endCol = 10; // 定义数据存放的结束列
                        int currentCol = startCol; // 当前处理的列从起始列开始
                        for (int i = 0; i < ROOdata.size() && i<20; i++) {
                            QModelIndex index = sd8825model->index(counter, 11); // 第11列的索引
                            QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                            QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                            sd8825model->setData(index, finalString);
                            counter += 2; // 每隔一个单元格设置数据

                            for (int j = 0; j < 8; j++) {
                                bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                QString binaryString = bit ? "1" : "0";
                                sd8825model->setData(sd8825model->index(row, currentCol), binaryString);
                                currentCol++;
                                if (currentCol > endCol) {
                                    currentCol = startCol;
                                    row+=2;//隔一行存放数据
                                }
                            }
                        }
                        QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                        qDebug() << "读10H："<<ROOdata.size();
                    }
                }
            });

        }else if (text=="SD8200") {
            sd8200Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x72);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                    if(ROOdata.size()==18)
                  {
                        // 解析并设置BCD码数据
                        int row = 1;
                        int counter = 1;//行计数
                        int startCol = 3; // 定义数据存放的起始列
                        int endCol = 10; // 定义数据存放的结束列
                        int currentCol = startCol; // 当前处理的列从起始列开始
                        for (int i = 0; i < ROOdata.size() && i<17; i++) {
                            QModelIndex index = sd8200model->index(counter, 11); // 第11列的索引
                            QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                            QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                            sd8200model->setData(index, finalString);
                            counter += 2; // 每隔一个单元格设置数据

                            for (int j = 0; j < 8; j++) {
                                bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                QString binaryString = bit ? "1" : "0";
                                sd8200model->setData(sd8200model->index(row, currentCol), binaryString);
                                currentCol++;
                                if (currentCol > endCol) {
                                    currentCol = startCol;
                                    row+=2;//隔一行存放数据
                                }
                            }
                        }
                        QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                        qDebug() << "读10H："<<ROOdata.size();
                    }
                }
            });

        }else if (text=="SD8804") {
            sd8804Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x3B);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                    if(ROOdata.size()==17)
                  {
                        // 解析并设置BCD码数据
                        int row = 1;
                        int counter = 1;//行计数
                        int startCol = 3; // 定义数据存放的起始列
                        int endCol = 10; // 定义数据存放的结束列
                        int currentCol = startCol; // 当前处理的列从起始列开始
                        for (int i = 0; i < ROOdata.size() && i<16; i++) {
                            QModelIndex index = sd8804model->index(counter, 11); // 第11列的索引
                            QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                            QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                            sd8804model->setData(index, finalString);
                            counter += 2; // 每隔一个单元格设置数据

                            for (int j = 0; j < 8; j++) {
                                bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                QString binaryString = bit ? "1" : "0";
                                sd8804model->setData(sd8804model->index(row, currentCol), binaryString);
                                currentCol++;
                                if (currentCol > endCol) {
                                    currentCol = startCol;
                                    row+=2;//隔一行存放数据
                                }
                            }
                        }
                        QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                        qDebug() << "读10H："<<ROOdata.size();
                    }
                }
            });

        }else if (text=="SD8840") {
            sd8840Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x92);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                    if(ROOdata.size()==17)
                  {
                        // 解析并设置BCD码数据
                        int row = 1;
                        int counter = 1;//行计数
                        int startCol = 3; // 定义数据存放的起始列
                        int endCol = 10; // 定义数据存放的结束列
                        int currentCol = startCol; // 当前处理的列从起始列开始
                        for (int i = 0; i < ROOdata.size() && i<16; i++) {
                            QModelIndex index = sd8840model->index(counter, 11); // 第11列的索引
                            QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                            QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                            sd8840model->setData(index, finalString);
                            counter += 2; // 每隔一个单元格设置数据

                            for (int j = 0; j < 8; j++) {
                                bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                QString binaryString = bit ? "1" : "0";
                                sd8840model->setData(sd8840model->index(row, currentCol), binaryString);
                                currentCol++;
                                if (currentCol > endCol) {
                                    currentCol = startCol;
                                    row+=2;//隔一行存放数据
                                }
                            }
                        }
                        QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                        qDebug() << "读10H："<<ROOdata.size();
                    }
                }
            });

        }else if (text=="SD8902") {
            sd8902Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0xA0);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                    if(ROOdata.size()==8)
                  {
                        // 解析并设置BCD码数据
                        int row = 1;
                        int counter = 1;//行计数
                        int startCol = 3; // 定义数据存放的起始列
                        int endCol = 10; // 定义数据存放的结束列
                        int currentCol = startCol; // 当前处理的列从起始列开始
                        for (int i = 0; i < ROOdata.size() && i<7; i++) {
                            QModelIndex index = sd8902model->index(counter, 11); // 第11列的索引
                            QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                            QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                            sd8902model->setData(index, finalString);
                            counter += 2; // 每隔一个单元格设置数据

                            for (int j = 0; j < 8; j++) {
                                bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                QString binaryString = bit ? "1" : "0";
                                sd8902model->setData(sd8902model->index(row, currentCol), binaryString);
                                currentCol++;
                                if (currentCol > endCol) {
                                    currentCol = startCol;
                                    row+=2;//隔一行存放数据
                                }
                            }
                        }
                        QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                        qDebug() << "读10H："<<ROOdata.size();
                    }
                }
            });

        }else if (text=="SD2010") {
            sd2010Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            uint16_t value = 3;
            QByteArray valueBytes(reinterpret_cast<const char*>(&value), sizeof(value));
            hexData.append(valueBytes);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                   if(ROOdata.size()==21)
                   {
                       // 解析并设置BCD码数据
                           int row = 1;
                           int counter = 1;//行计数
                           int startCol = 3; // 定义数据存放的起始列
                           int endCol = 10; // 定义数据存放的结束列
                           int currentCol = startCol; // 当前处理的列从起始列开始
                           for (int i = 0; i < ROOdata.size() && i < 20; i++) {
                               QModelIndex index = sd2010model->index(counter, 11); // 第11列的索引
                               QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                               QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                               sd2010model->setData(index, finalString);
                               counter += 2; // 每隔一个单元格设置数据

                               for (int j = 0; j < 8; j++) {
                                   bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                   QString binaryString = bit ? "1" : "0";
                                   sd2010model->setData(sd2010model->index(row, currentCol), binaryString);
                                   currentCol++;
                                   if (currentCol > endCol) {
                                       currentCol = startCol;
                                       row+=2;//隔一行存放数据
                                   }
                               }
                           }
                       QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                       qDebug() << "读00H："<<ROOdata.size();
                  }
               }
            });

        }else if (text=="SD2058") {
            sd2058Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0xFA);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                   if(ROOdata.size()==21)
                   {
                       // 解析并设置BCD码数据
                           int row = 1;
                           int counter = 1;//行计数
                           int startCol = 3; // 定义数据存放的起始列
                           int endCol = 10; // 定义数据存放的结束列
                           int currentCol = startCol; // 当前处理的列从起始列开始
                           for (int i = 0; i < ROOdata.size() && i < 20; i++) {
                               QModelIndex index = sd2058model->index(counter, 11); // 第11列的索引
                               QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                               QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                               sd2058model->setData(index, finalString);
                               counter += 2; // 每隔一个单元格设置数据

                               for (int j = 0; j < 8; j++) {
                                   bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                   QString binaryString = bit ? "1" : "0";
                                   sd2058model->setData(sd2058model->index(row, currentCol), binaryString);
                                   currentCol++;
                                   if (currentCol > endCol) {
                                       currentCol = startCol;
                                       row+=2;//隔一行存放数据
                                   }
                               }
                           }
                       QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                       qDebug() << "读00H："<<ROOdata.size();
                  }
               }
            });
        }else if (text=="SD2059") {
            sd2059Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0xFA);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                   if(ROOdata.size()==21)
                   {
                       // 解析并设置BCD码数据
                           int row = 1;
                           int counter = 1;//行计数
                           int startCol = 3; // 定义数据存放的起始列
                           int endCol = 10; // 定义数据存放的结束列
                           int currentCol = startCol; // 当前处理的列从起始列开始
                           for (int i = 0; i < ROOdata.size() && i < 20; i++) {
                               QModelIndex index = sd2059model->index(counter, 11); // 第11列的索引
                               QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                               QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                               sd2059model->setData(index, finalString);
                               counter += 2; // 每隔一个单元格设置数据

                               for (int j = 0; j < 8; j++) {
                                   bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                   QString binaryString = bit ? "1" : "0";
                                   sd2059model->setData(sd2059model->index(row, currentCol), binaryString);
                                   currentCol++;
                                   if (currentCol > endCol) {
                                       currentCol = startCol;
                                       row+=2;//隔一行存放数据
                                   }
                               }
                           }
                       QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                       qDebug() << "读00H："<<ROOdata.size();
                  }
               }
            });
        }else if (text=="SD2068"||text=="SD2400"||text=="SD2401"||text=="SD2402"||text=="SD2403"||text=="SD2404"||text=="SD2405") {
            sd2068Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0xFA);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                   if(ROOdata.size()==21)
                   {
                       // 解析并设置BCD码数据
                           int row = 1;
                           int counter = 1;//行计数
                           int startCol = 3; // 定义数据存放的起始列
                           int endCol = 10; // 定义数据存放的结束列
                           int currentCol = startCol; // 当前处理的列从起始列开始
                           for (int i = 0; i < ROOdata.size() && i < 20; i++) {
                               QModelIndex index = sd2068model->index(counter, 11); // 第11列的索引
                               QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                               QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                               sd2068model->setData(index, finalString);
                               counter += 2; // 每隔一个单元格设置数据

                               for (int j = 0; j < 8; j++) {
                                   bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                   QString binaryString = bit ? "1" : "0";
                                   sd2068model->setData(sd2068model->index(row, currentCol), binaryString);
                                   currentCol++;
                                   if (currentCol > endCol) {
                                       currentCol = startCol;
                                       row+=2;//隔一行存放数据
                                   }
                               }
                           }
                       QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                       qDebug() << "读00H："<<ROOdata.size();
                  }
               }
            });

        }else if (text=="SD2069") {
            sd2069Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0xFA);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                   if(ROOdata.size()==21)
                   {
                       // 解析并设置BCD码数据
                           int row = 1;
                           int counter = 1;//行计数
                           int startCol = 3; // 定义数据存放的起始列
                           int endCol = 10; // 定义数据存放的结束列
                           int currentCol = startCol; // 当前处理的列从起始列开始
                           for (int i = 0; i < ROOdata.size() && i < 20; i++) {
                               QModelIndex index = sd2069model->index(counter, 11); // 第11列的索引
                               QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                               QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                               sd2069model->setData(index, finalString);
                               counter += 2; // 每隔一个单元格设置数据

                               for (int j = 0; j < 8; j++) {
                                   bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                   QString binaryString = bit ? "1" : "0";
                                   sd2069model->setData(sd2069model->index(row, currentCol), binaryString);
                                   currentCol++;
                                   if (currentCol > endCol) {
                                       currentCol = startCol;
                                       row+=2;//隔一行存放数据
                                   }
                               }
                           }
                       QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                       qDebug() << "读00H："<<ROOdata.size();
                  }
               }
            });

        }else if (text=="SD2077") {
            sd2077Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0xFA);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                   if(ROOdata.size()==21)
                   {
                       // 解析并设置BCD码数据
                           int row = 1;
                           int counter = 1;//行计数
                           int startCol = 3; // 定义数据存放的起始列
                           int endCol = 10; // 定义数据存放的结束列
                           int currentCol = startCol; // 当前处理的列从起始列开始
                           for (int i = 0; i < ROOdata.size() && i < 20; i++) {
                               QModelIndex index = sd2077model->index(counter, 11); // 第11列的索引
                               QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                               QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                               sd2077model->setData(index, finalString);
                               counter += 2; // 每隔一个单元格设置数据

                               for (int j = 0; j < 8; j++) {
                                   bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                   QString binaryString = bit ? "1" : "0";
                                   sd2077model->setData(sd2077model->index(row, currentCol), binaryString);
                                   currentCol++;
                                   if (currentCol > endCol) {
                                       currentCol = startCol;
                                       row+=2;//隔一行存放数据
                                   }
                               }
                           }
                       QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                       qDebug() << "读00H："<<ROOdata.size();
                  }
               }
            });

        }else if (text=="SD2078") {
            sd2078Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0xFA);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                   if(ROOdata.size()==21)
                   {
                       // 解析并设置BCD码数据
                           int row = 1;
                           int counter = 1;//行计数
                           int startCol = 3; // 定义数据存放的起始列
                           int endCol = 10; // 定义数据存放的结束列
                           int currentCol = startCol; // 当前处理的列从起始列开始
                           for (int i = 0; i < ROOdata.size() && i < 20; i++) {
                               QModelIndex index = sd2078model->index(counter, 11); // 第11列的索引
                               QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                               QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                               sd2078model->setData(index, finalString);
                               counter += 2; // 每隔一个单元格设置数据

                               for (int j = 0; j < 8; j++) {
                                   bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                   QString binaryString = bit ? "1" : "0";
                                   sd2078model->setData(sd2078model->index(row, currentCol), binaryString);
                                   currentCol++;
                                   if (currentCol > endCol) {
                                       currentCol = startCol;
                                       row+=2;//隔一行存放数据
                                   }
                               }
                           }
                       QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                       qDebug() << "读00H："<<ROOdata.size();
                  }
               }

            });

        }else if (text=="SD2200"||text=="SD2201"||text=="SD2202"||text=="SD2203"||text=="SD2204")
        {
            sd2200Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            uint16_t value = 10;
            QByteArray valueBytes(reinterpret_cast<const char*>(&value), sizeof(value));
            hexData.append(valueBytes);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                    if (ROOdata.size() == 19)
                    {
                        // 解析并设置BCD码数据
                        int row = 1;
                        int counter = 1; // 行计数
                        int startCol = 3; // 定义数据存放的起始列
                        int endCol = 10; // 定义数据存放的结束列
                        int currentCol = startCol; // 当前处理的列从起始列开始

                        for (int i = 0; i < ROOdata.size() && i < 19; i++) {
                            QModelIndex index = sd2200model->index(counter, 11); // 第11列的索引
                            QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                            QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                            sd2200model->setData(index, finalString);
                            counter += 2; // 每隔一个单元格设置数据

                            // 处理位的部分
                            for (int j = 0; j < 8; j++)
                            {
                                bool bit;

                                // 判断当前元素的索引i是否在3到9的范围内
                                if (i >= 2 && i <= 8)
                                { // 注意这里i从0开始，所以第3个是i=2，第9个是i=8
                                    bit = (ROOdata.at(i) >> j) & 1; // 从低位到高位
                                } else {
                                    bit = (ROOdata.at(i) >> (7 - j)) & 1; // 从高位到低位
                                }

                                QString binaryString = bit ? "1" : "0";
                                sd2200model->setData(sd2200model->index(row, currentCol), binaryString);
                                currentCol++;

                                if (currentCol > endCol)
                                {
                                    currentCol = startCol;
                                    row += 2; // 隔一行存放数据
                                }
                            }
                        }
                        QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                        qDebug() << "读00H：" << ROOdata.size();
                    }
               }
            });
        }

       }else{
        QMessageBox::information(this,"错误","串口打开失败，串口未连接或者被占用!");
    }
}

/**
功能：读3按键--读取芯片的寄存器，具体寄存器根据具体芯片型号来确定
修改：sjm
参数：-
返回值：-
**/
void RtcMessage::on_R20H_clicked()
{
    QString text = ui->rtctype->toPlainText();
    //清空输入缓冲区
    serialPort->clear(QSerialPort::Input);

    if(serialPort->isOpen())
    {
        if(text=="SD3068"||text=="SD3010"||text=="SD3025"||text=="SD3031"||text=="SD3077"||text=="SD3078"||text=="SD3178"||text=="SD3900"||text=="SD2503"||text=="SD2505"||text=="SD2506"||text=="SD2507"||text=="SD2508"||text=="SD2520"||text=="SD2522"||text=="SD2523")
        {
            sd3068Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0xC1);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray R20data=serialPort->readAll();
                qDebug()<<"收到数据："<<R20data;
                qDebug() <<R20data.size();

                if (!R20data.isEmpty())
                {
                    if(R20data.size()==33)
                    {
                        // 解析并设置BCD码数据
                       int row = 64;
                       int counter = 64;//行计数
                       int startCol = 3; // 定义数据存放的起始列
                       int endCol = 10; // 定义数据存放的结束列
                       int currentCol = startCol; // 当前处理的列从起始列开始
                       for (int i = 0; i < R20data.size() && i < 32; i++) {
                           QModelIndex index = sd3068model->index(counter, 11); // 第11列的索引
                           QString hexString = QString::number(static_cast<quint8>(R20data[i]), 16).toUpper();
                           QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                           sd3068model->setData(index, finalString);
                           counter++;

                           for (int j = 0; j < 8; j++) {
                               bool bit = (R20data.at(i) >> (7 - j)) & 1;
                               QString binaryString = bit ? "1" : "0";
                               sd3068model->setData(sd3068model->index(row, currentCol), binaryString);
                               currentCol++;
                               if (currentCol > endCol) {
                                   currentCol = startCol;
                                   row++;
                               }
                           }
                       }
                       QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                       qDebug() << "读00H："<<R20data.size();
                    }

                }
            });

        }else if (text=="SD8810") {
            sd8810Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x18);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray R20data=serialPort->readAll();
                qDebug()<<"收到数据："<<R20data;
                qDebug() <<R20data.size();

                if (!R20data.isEmpty())
                {
                    if(R20data.size()==23)
                    {
                        // 解析并设置BCD码数据
                            int row = 31;
                            int counter = 31;//行计数
                            int startCol = 3; // 定义数据存放的起始列
                            int endCol = 10; // 定义数据存放的结束列
                            int currentCol = startCol; // 当前处理的列从起始列开始
                            for (int i = 0; i < R20data.size() && i < 22; i++) {
                                QModelIndex index = sd8810model->index(counter, 11); // 第11列的索引
                                QString hexString = QString::number(static_cast<quint8>(R20data[i]), 16).toUpper();
                                QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                                sd8810model->setData(index, finalString);
                                counter +=2;

                                for (int j = 0; j < 8; j++) {
                                    bool bit = (R20data.at(i) >> (7 - j)) & 1;
                                    QString binaryString = bit ? "1" : "0";
                                    sd8810model->setData(sd8810model->index(row, currentCol), binaryString);
                                    currentCol++;
                                    if (currentCol > endCol) {
                                        currentCol = startCol;
                                        row+=2;//隔一行存放数据
                                    }
                                }
                            }
                            QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                            qDebug() << "读20H："<<R20data.size();
                    }
                }
            });

        }else if (text=="SD8939") {
            sd8939Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x30);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                    if(ROOdata.size()==79)
                  {
                        // 解析并设置BCD码数据
                        int row = 41;
                        int counter = 41;//行计数
                        int startCol = 3; // 定义数据存放的起始列
                        int endCol = 10; // 定义数据存放的结束列
                        int currentCol = startCol; // 当前处理的列从起始列开始
                        for (int i = 0; i < ROOdata.size() && i<78; i++) {
                            QModelIndex index = sd8939model->index(counter, 11); // 第11列的索引
                            QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                            QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                            sd8939model->setData(index, finalString);
                            counter += 2; // 每隔一个单元格设置数据

                            for (int j = 0; j < 8; j++) {
                                bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                QString binaryString = bit ? "1" : "0";
                                sd8939model->setData(sd8939model->index(row, currentCol), binaryString);
                                currentCol++;
                                if (currentCol > endCol) {
                                    currentCol = startCol;
                                    row+=2;//隔一行存放数据
                                }
                            }
                        }
                        QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                        qDebug() << "读10H："<<ROOdata.size();
                    }
                }
            });

        }else if (text=="SD8800") {
            sd8800Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x40);
            serialPort->write(hexData);

            QTimer::singleShot(110, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                    if(ROOdata.size()==12)
                  {
                        // 解析并设置BCD码数据
                        int row = 33;
                        int counter = 33;//行计数
                        int startCol = 3; // 定义数据存放的起始列
                        int endCol = 10; // 定义数据存放的结束列
                        int currentCol = startCol; // 当前处理的列从起始列开始
                        for (int i = 0; i < ROOdata.size() && i<11; i++) {
                            QModelIndex index = sd8800model->index(counter, 11); // 第11列的索引
                            QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                            QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                            sd8800model->setData(index, finalString);
                            counter += 2; // 每隔一个单元格设置数据

                            for (int j = 0; j < 8; j++) {
                                bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                QString binaryString = bit ? "1" : "0";
                                sd8800model->setData(sd8800model->index(row, currentCol), binaryString);
                                currentCol++;
                                if (currentCol > endCol) {
                                    currentCol = startCol;
                                    row+=2;//隔一行存放数据
                                }
                            }
                        }
                        QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                        qDebug() << "读10H："<<ROOdata.size();
                    }
                }
            });

        }else if (text=="SD8931") {
            sd8931Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x52);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                    if(ROOdata.size()==79)
                  {
                        // 解析并设置BCD码数据
                        int row = 41;
                        int counter = 41;//行计数
                        int startCol = 3; // 定义数据存放的起始列
                        int endCol = 10; // 定义数据存放的结束列
                        int currentCol = startCol; // 当前处理的列从起始列开始
                        for (int i = 0; i < ROOdata.size() && i<78; i++) {
                            QModelIndex index = sd8931model->index(counter, 11); // 第11列的索引
                            QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                            QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                            sd8931model->setData(index, finalString);
                            counter += 2; // 每隔一个单元格设置数据

                            for (int j = 0; j < 8; j++) {
                                bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                QString binaryString = bit ? "1" : "0";
                                sd8931model->setData(sd8931model->index(row, currentCol), binaryString);
                                currentCol++;
                                if (currentCol > endCol) {
                                    currentCol = startCol;
                                    row+=2;//隔一行存放数据
                                }
                            }
                        }
                        QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                        qDebug() << "读10H："<<ROOdata.size();
                    }
                }
            });

        }else if (text=="SD8825") {
            sd8825Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x64);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                    if(ROOdata.size()==78)
                  {
                        // 解析并设置BCD码数据
                        int row = 41;
                        int counter = 41;//行计数
                        int startCol = 3; // 定义数据存放的起始列
                        int endCol = 10; // 定义数据存放的结束列
                        int currentCol = startCol; // 当前处理的列从起始列开始
                        for (int i = 0; i < ROOdata.size() && i<77; i++) {
                            QModelIndex index = sd8825model->index(counter, 11); // 第11列的索引
                            QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                            QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                            sd8825model->setData(index, finalString);
                            counter += 2; // 每隔一个单元格设置数据

                            for (int j = 0; j < 8; j++) {
                                bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                QString binaryString = bit ? "1" : "0";
                                sd8825model->setData(sd8825model->index(row, currentCol), binaryString);
                                currentCol++;
                                if (currentCol > endCol) {
                                    currentCol = startCol;
                                    row+=2;//隔一行存放数据
                                }
                            }
                        }
                        QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                        qDebug() << "读10H："<<ROOdata.size();
                    }
                }
            });

        }else if (text=="SD8200") {
            sd8200Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x74);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                    if(ROOdata.size()==79)
                  {
                        // 解析并设置BCD码数据
                        int row = 35;
                        int counter = 35;//行计数
                        int startCol = 3; // 定义数据存放的起始列
                        int endCol = 10; // 定义数据存放的结束列
                        int currentCol = startCol; // 当前处理的列从起始列开始
                        for (int i = 0; i < ROOdata.size() && i<78; i++) {
                            QModelIndex index = sd8200model->index(counter, 11); // 第11列的索引
                            QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                            QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                            sd8200model->setData(index, finalString);
                            counter += 2; // 每隔一个单元格设置数据

                            for (int j = 0; j < 8; j++) {
                                bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                QString binaryString = bit ? "1" : "0";
                                sd8200model->setData(sd8200model->index(row, currentCol), binaryString);
                                currentCol++;
                                if (currentCol > endCol) {
                                    currentCol = startCol;
                                    row+=2;//隔一行存放数据
                                }
                            }
                        }
                        QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                        qDebug() << "读10H："<<ROOdata.size();
                    }
                }
            });

        }else if (text=="SD8804") {
            sd8804Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x3C);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                    if(ROOdata.size()==25)
                  {
                        // 解析并设置BCD码数据
                        int row = 33;
                        int counter = 33;//行计数
                        int startCol = 3; // 定义数据存放的起始列
                        int endCol = 10; // 定义数据存放的结束列
                        int currentCol = startCol; // 当前处理的列从起始列开始
                        for (int i = 0; i < ROOdata.size() && i<24; i++) {
                            QModelIndex index = sd8804model->index(counter, 11); // 第11列的索引
                            QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                            QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                            sd8804model->setData(index, finalString);
                            counter += 2; // 每隔一个单元格设置数据

                            for (int j = 0; j < 8; j++) {
                                bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                QString binaryString = bit ? "1" : "0";
                                sd8804model->setData(sd8804model->index(row, currentCol), binaryString);
                                currentCol++;
                                if (currentCol > endCol) {
                                    currentCol = startCol;
                                    row+=2;//隔一行存放数据
                                }
                            }
                        }
                        QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                        qDebug() << "读10H："<<ROOdata.size();
                    }
                }
            });

        }else if (text=="SD8840") {
            sd8840Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x94);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                    if(ROOdata.size()==10)
                  {
                        // 解析并设置BCD码数据
                        int row = 33;
                        int counter = 33;//行计数
                        int startCol = 3; // 定义数据存放的起始列
                        int endCol = 10; // 定义数据存放的结束列
                        int currentCol = startCol; // 当前处理的列从起始列开始
                        for (int i = 0; i < ROOdata.size() && i<9; i++) {
                            QModelIndex index = sd8840model->index(counter, 11); // 第11列的索引
                            QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                            QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                            sd8840model->setData(index, finalString);
                            counter += 2; // 每隔一个单元格设置数据

                            for (int j = 0; j < 8; j++) {
                                bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                QString binaryString = bit ? "1" : "0";
                                sd8840model->setData(sd8840model->index(row, currentCol), binaryString);
                                currentCol++;
                                if (currentCol > endCol) {
                                    currentCol = startCol;
                                    row+=2;//隔一行存放数据
                                }
                            }
                        }
                        QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                        qDebug() << "读10H："<<ROOdata.size();
                    }
                }
            });

        }else if (text=="SD8902") {
            sd8902Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0xA2);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                    if(ROOdata.size()==11)
                  {
                        // 解析并设置BCD码数据
                        int row = 15;
                        int counter = 15;//行计数
                        int startCol = 3; // 定义数据存放的起始列
                        int endCol = 10; // 定义数据存放的结束列
                        int currentCol = startCol; // 当前处理的列从起始列开始
                        for (int i = 0; i < ROOdata.size() && i<10; i++) {
                            QModelIndex index = sd8902model->index(counter, 11); // 第11列的索引
                            QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                            QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                            sd8902model->setData(index, finalString);
                            counter += 2; // 每隔一个单元格设置数据

                            for (int j = 0; j < 8; j++) {
                                bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                QString binaryString = bit ? "1" : "0";
                                sd8902model->setData(sd8902model->index(row, currentCol), binaryString);
                                currentCol++;
                                if (currentCol > endCol) {
                                    currentCol = startCol;
                                    row+=2;//隔一行存放数据
                                }
                            }
                        }
                        QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                        qDebug() << "读10H："<<ROOdata.size();
                    }
                }
            });

        }else if (text=="SD2010") {
            sd2010Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            uint16_t value = 5;
            QByteArray valueBytes(reinterpret_cast<const char*>(&value), sizeof(value));
            hexData.append(valueBytes);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                   if(ROOdata.size()==13)
                   {
                       // 解析并设置BCD码数据
                           int row = 41;
                           int counter = 41;//行计数
                           int startCol = 3; // 定义数据存放的起始列
                           int endCol = 10; // 定义数据存放的结束列
                           int currentCol = startCol; // 当前处理的列从起始列开始
                           for (int i = 0; i < ROOdata.size() && i < 12; i++) {
                               QModelIndex index = sd2010model->index(counter, 11); // 第11列的索引
                               QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                               QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                               sd2010model->setData(index, finalString);
                               counter += 2; // 每隔一个单元格设置数据

                               for (int j = 0; j < 8; j++) {
                                   bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                   QString binaryString = bit ? "1" : "0";
                                   sd2010model->setData(sd2010model->index(row, currentCol), binaryString);
                                   currentCol++;
                                   if (currentCol > endCol) {
                                       currentCol = startCol;
                                       row+=2;//隔一行存放数据
                                   }
                               }
                           }
                       QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                       qDebug() << "读00H："<<ROOdata.size();
                  }
               }
            });

        }else if (text=="SD2058") {
            sd2058Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0xFB);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                   if(ROOdata.size()==45)
                   {
                       // 解析并设置BCD码数据
                           int row = 41;
                           int counter = 41;//行计数
                           int startCol = 3; // 定义数据存放的起始列
                           int endCol = 10; // 定义数据存放的结束列
                           int currentCol = startCol; // 当前处理的列从起始列开始
                           for (int i = 0; i < ROOdata.size() && i < 44; i++) {
                               QModelIndex index = sd2058model->index(counter, 11); // 第11列的索引
                               QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                               QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                               sd2058model->setData(index, finalString);
                               counter += 2; // 每隔一个单元格设置数据

                               for (int j = 0; j < 8; j++) {
                                   bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                   QString binaryString = bit ? "1" : "0";
                                   sd2058model->setData(sd2058model->index(row, currentCol), binaryString);
                                   currentCol++;
                                   if (currentCol > endCol) {
                                       currentCol = startCol;
                                       row+=2;//隔一行存放数据
                                   }
                               }
                           }
                       QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                       qDebug() << "读00H："<<ROOdata.size();
                  }
               }
            });

        }else if (text=="SD2059") {
            sd2059Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0xFB);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                   if(ROOdata.size()==45)
                   {
                       // 解析并设置BCD码数据
                           int row = 41;
                           int counter = 41;//行计数
                           int startCol = 3; // 定义数据存放的起始列
                           int endCol = 10; // 定义数据存放的结束列
                           int currentCol = startCol; // 当前处理的列从起始列开始
                           for (int i = 0; i < ROOdata.size() && i < 44; i++) {
                               QModelIndex index = sd2059model->index(counter, 11); // 第11列的索引
                               QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                               QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                               sd2059model->setData(index, finalString);
                               counter += 2; // 每隔一个单元格设置数据

                               for (int j = 0; j < 8; j++) {
                                   bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                   QString binaryString = bit ? "1" : "0";
                                   sd2059model->setData(sd2059model->index(row, currentCol), binaryString);
                                   currentCol++;
                                   if (currentCol > endCol) {
                                       currentCol = startCol;
                                       row+=2;//隔一行存放数据
                                   }
                               }
                           }
                       QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                       qDebug() << "读00H："<<ROOdata.size();
                  }
               }
            });

        }else if (text=="SD2068"||text=="SD2400"||text=="SD2401"||text=="SD2402"||text=="SD2403"||text=="SD2404"||text=="SD2405") {
            sd2068Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0xFC);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                   if(ROOdata.size()==13)
                   {
                            // 解析并设置BCD码数据
                           int row = 41;
                           int counter = 41;//行计数
                           int startCol = 3; // 定义数据存放的起始列
                           int endCol = 10; // 定义数据存放的结束列
                           int currentCol = startCol; // 当前处理的列从起始列开始
                           for (int i = 0; i < ROOdata.size() && i < 12; i++) {
                               QModelIndex index = sd2068model->index(counter, 11); // 第11列的索引
                               QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                               QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                               sd2068model->setData(index, finalString);
                               counter += 2; // 每隔一个单元格设置数据

                               for (int j = 0; j < 8; j++) {
                                   bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                   QString binaryString = bit ? "1" : "0";
                                   sd2068model->setData(sd2068model->index(row, currentCol), binaryString);
                                   currentCol++;
                                   if (currentCol > endCol) {
                                       currentCol = startCol;
                                       row+=2;//隔一行存放数据
                                   }
                               }
                           }
                       QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                       qDebug() << "读00H："<<ROOdata.size();
                  }
               }
            });
        }else if (text=="SD2069") {
            sd2069Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0xFC);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                   if(ROOdata.size()==13)
                   {
                            // 解析并设置BCD码数据
                           int row = 41;
                           int counter = 41;//行计数
                           int startCol = 3; // 定义数据存放的起始列
                           int endCol = 10; // 定义数据存放的结束列
                           int currentCol = startCol; // 当前处理的列从起始列开始
                           for (int i = 0; i < ROOdata.size() && i < 12; i++) {
                               QModelIndex index = sd2069model->index(counter, 11); // 第11列的索引
                               QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                               QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                               sd2069model->setData(index, finalString);
                               counter += 2; // 每隔一个单元格设置数据

                               for (int j = 0; j < 8; j++) {
                                   bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                   QString binaryString = bit ? "1" : "0";
                                   sd2069model->setData(sd2069model->index(row, currentCol), binaryString);
                                   currentCol++;
                                   if (currentCol > endCol) {
                                       currentCol = startCol;
                                       row+=2;//隔一行存放数据
                                   }
                               }
                           }
                       QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                       qDebug() << "读00H："<<ROOdata.size();
                  }
               }
            });
        }else if (text=="SD2077") {
            sd2077Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0xFC);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                   if(ROOdata.size()==13)
                   {
                            // 解析并设置BCD码数据
                           int row = 41;
                           int counter = 41;//行计数
                           int startCol = 3; // 定义数据存放的起始列
                           int endCol = 10; // 定义数据存放的结束列
                           int currentCol = startCol; // 当前处理的列从起始列开始
                           for (int i = 0; i < ROOdata.size() && i < 12; i++) {
                               QModelIndex index = sd2077model->index(counter, 11); // 第11列的索引
                               QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                               QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                               sd2077model->setData(index, finalString);
                               counter += 2; // 每隔一个单元格设置数据

                               for (int j = 0; j < 8; j++) {
                                   bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                   QString binaryString = bit ? "1" : "0";
                                   sd2077model->setData(sd2077model->index(row, currentCol), binaryString);
                                   currentCol++;
                                   if (currentCol > endCol) {
                                       currentCol = startCol;
                                       row+=2;//隔一行存放数据
                                   }
                               }
                           }
                       QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                       qDebug() << "读00H："<<ROOdata.size();
                  }
               }
            });
        }else if (text=="SD2078") {
            sd2078Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0xFC);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                   if(ROOdata.size()==13)
                   {
                            // 解析并设置BCD码数据
                           int row = 41;
                           int counter = 41;//行计数
                           int startCol = 3; // 定义数据存放的起始列
                           int endCol = 10; // 定义数据存放的结束列
                           int currentCol = startCol; // 当前处理的列从起始列开始
                           for (int i = 0; i < ROOdata.size() && i < 12; i++) {
                               QModelIndex index = sd2078model->index(counter, 11); // 第11列的索引
                               QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                               QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                               sd2078model->setData(index, finalString);
                               counter += 2; // 每隔一个单元格设置数据

                               for (int j = 0; j < 8; j++) {
                                   bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                   QString binaryString = bit ? "1" : "0";
                                   sd2078model->setData(sd2078model->index(row, currentCol), binaryString);
                                   currentCol++;
                                   if (currentCol > endCol) {
                                       currentCol = startCol;
                                       row+=2;//隔一行存放数据
                                   }
                               }
                           }
                       QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                       qDebug() << "读00H："<<ROOdata.size();
                  }
               }
            });
        }

       }else{
        QMessageBox::information(this,"错误","串口打开失败，串口未连接或者被占用!");
    }

}

/**
功能：读4按键--读取芯片的寄存器，具体寄存器根据具体芯片型号来确定
修改：sjm
参数：-
返回值：-
**/
void RtcMessage::on_R40H_clicked()
{
    QString text = ui->rtctype->toPlainText();
    //清空输入缓冲区
    serialPort->clear(QSerialPort::Input);

    if(serialPort->isOpen())
    {
        if(text=="SD3068"||text=="SD3010"||text=="SD3025"||text=="SD3031"||text=="SD3077"||text=="SD3078"||text=="SD3178"||text=="SD3900"||text=="SD2503"||text=="SD2505"||text=="SD2506"||text=="SD2507"||text=="SD2508"||text=="SD2520"||text=="SD2522"||text=="SD2523")
        {
            sd3068Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0xAE);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray R40data=serialPort->readAll();
                qDebug()<<"收到数据："<<R40data;
                qDebug() <<R40data.size();

                if (!R40data.isEmpty())
                {
                    if(R40data.size()==33)
                    {
                        // 解析并设置BCD码数据
                            int row = 96;
                            int counter = 96;//行计数
                            int startCol = 3; // 定义数据存放的起始列
                            int endCol = 10; // 定义数据存放的结束列
                            int currentCol = startCol; // 当前处理的列从起始列开始
                            for (int i = 0; i < R40data.size() && i < 32; i++){
                                QModelIndex index = sd3068model->index(counter, 11); // 第11列的索引
                                QString hexString = QString::number(static_cast<quint8>(R40data[i]), 16).toUpper();
                                QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                                sd3068model->setData(index, finalString);
                                counter++;

                                for (int j = 0; j < 8; j++) {
                                    bool bit = (R40data.at(i) >> (7 - j)) & 1;
                                    QString binaryString = bit ? "1" : "0";
                                    sd3068model->setData(sd3068model->index(row, currentCol), binaryString);
                                    currentCol++;
                                    if (currentCol > endCol) {
                                        currentCol = startCol;
                                        row++;
                                    }
                                }
                            }
                            QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                            qDebug() << "读寄存器40H："<<R40data.size();
                    }
                }
            });

        }else if (text=="SD8810") {
            sd8810Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x20);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray R20data=serialPort->readAll();
                qDebug()<<"收到数据："<<R20data;
                qDebug() <<R20data.size();

                if (!R20data.isEmpty())
                {
                    if(R20data.size()==63)
                    {
                        // 解析并设置BCD码数据
                            int row = 75;
                            int counter = 75;//行计数
                            int startCol = 3; // 定义数据存放的起始列
                            int endCol = 10; // 定义数据存放的结束列
                            int currentCol = startCol; // 当前处理的列从起始列开始
                            for (int i = 0; i < R20data.size() && i < 62; i++) {
                                QModelIndex index = sd8810model->index(counter, 11); // 第11列的索引
                                QString hexString = QString::number(static_cast<quint8>(R20data[i]), 16).toUpper();
                                QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                                sd8810model->setData(index, finalString);
                                counter +=2;

                                for (int j = 0; j < 8; j++) {
                                    bool bit = (R20data.at(i) >> (7 - j)) & 1;
                                    QString binaryString = bit ? "1" : "0";
                                    sd8810model->setData(sd8810model->index(row, currentCol), binaryString);
                                    currentCol++;
                                    if (currentCol > endCol) {
                                        currentCol = startCol;
                                        row+=2;//隔一行存放数据
                                    }
                                }
                            }
                            QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                            qDebug() << "读20H："<<R20data.size();
                    }
                }
            });

        }else if (text=="SD8939") {
            sd8939Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x32);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                    if(ROOdata.size()==5)
                  {
                        // 解析并设置BCD码数据
                        int row = 197;
                        int counter = 197;//行计数
                        int startCol = 3; // 定义数据存放的起始列
                        int endCol = 10; // 定义数据存放的结束列
                        int currentCol = startCol; // 当前处理的列从起始列开始
                        for (int i = 0; i < ROOdata.size() && i<4; i++) {
                            QModelIndex index = sd8939model->index(counter, 11); // 第11列的索引
                            QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                            QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                            sd8939model->setData(index, finalString);
                            counter += 2; // 每隔一个单元格设置数据

                            for (int j = 0; j < 8; j++) {
                                bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                QString binaryString = bit ? "1" : "0";
                                sd8939model->setData(sd8939model->index(row, currentCol), binaryString);
                                currentCol++;
                                if (currentCol > endCol) {
                                    currentCol = startCol;
                                    row+=2;//隔一行存放数据
                                }
                            }
                        }
                        QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                        qDebug() << "读10H："<<ROOdata.size();
                    }
                }
            });

        }else if (text=="SD8800") {
            sd8800Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x42);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                    if(ROOdata.size()==110)
                  {
                        // 解析并设置BCD码数据
                        int row = 54;
                        int counter = 54;//行计数
                        int startCol = 3; // 定义数据存放的起始列
                        int endCol = 10; // 定义数据存放的结束列
                        int currentCol = startCol; // 当前处理的列从起始列开始
                        for (int i = 0; i < ROOdata.size() && i<109; i++) {

                            QModelIndex index = sd8800model->index(counter, 11); // 第11列的索引
                            QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                            QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                            sd8800model->setData(index, finalString);
                            counter++; // 每隔一个单元格设置数据

                            for (int j = 0; j < 8; j++) {
                                bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                QString binaryString = bit ? "1" : "0";
                                sd8800model->setData(sd8800model->index(row, currentCol), binaryString);
                                currentCol++;
                                if (currentCol > endCol) {
                                    currentCol = startCol;
                                    row++;//隔一行存放数据
                                }
                            }
                        }
                        QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                        qDebug() << "读10H："<<ROOdata.size();
                    }
                }
            });

        }else if (text=="SD8931") {
            sd8931Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x54);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                    if(ROOdata.size()==5)
                  {
                        // 解析并设置BCD码数据
                        int row = 197;
                        int counter = 197;//行计数
                        int startCol = 3; // 定义数据存放的起始列
                        int endCol = 10; // 定义数据存放的结束列
                        int currentCol = startCol; // 当前处理的列从起始列开始
                        for (int i = 0; i < ROOdata.size() && i<4; i++) {
                            QModelIndex index = sd8931model->index(counter, 11); // 第11列的索引
                            QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                            QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                            sd8931model->setData(index, finalString);
                            counter += 2; // 每隔一个单元格设置数据

                            for (int j = 0; j < 8; j++) {
                                bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                QString binaryString = bit ? "1" : "0";
                                sd8931model->setData(sd8931model->index(row, currentCol), binaryString);
                                currentCol++;
                                if (currentCol > endCol) {
                                    currentCol = startCol;
                                    row+=2;//隔一行存放数据
                                }
                            }
                        }
                        QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                        qDebug() << "读10H："<<ROOdata.size();
                    }
                }
            });

        }else if (text=="SD8825") {
            sd8825Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x66);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                    if(ROOdata.size()==5)
                  {
                        // 解析并设置BCD码数据
                        int row = 197;
                        int counter = 197;//行计数
                        int startCol = 3; // 定义数据存放的起始列
                        int endCol = 10; // 定义数据存放的结束列
                        int currentCol = startCol; // 当前处理的列从起始列开始
                        for (int i = 0; i < ROOdata.size() && i<4; i++) {
                            QModelIndex index = sd8825model->index(counter, 11); // 第11列的索引
                            QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                            QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                            sd8825model->setData(index, finalString);
                            counter += 2; // 每隔一个单元格设置数据

                            for (int j = 0; j < 8; j++) {
                                bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                QString binaryString = bit ? "1" : "0";
                                sd8825model->setData(sd8825model->index(row, currentCol), binaryString);
                                currentCol++;
                                if (currentCol > endCol) {
                                    currentCol = startCol;
                                    row+=2;//隔一行存放数据
                                }
                            }
                        }
                        QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                        qDebug() << "读10H："<<ROOdata.size();
                    }
                }
            });

        }else if (text=="SD8200") {
            sd8200Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x76);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                    if(ROOdata.size()==5)
                  {
                        // 解析并设置BCD码数据
                        int row = 191;
                        int counter = 191;//行计数
                        int startCol = 3; // 定义数据存放的起始列
                        int endCol = 10; // 定义数据存放的结束列
                        int currentCol = startCol; // 当前处理的列从起始列开始
                        for (int i = 0; i < ROOdata.size() && i<4; i++) {
                            QModelIndex index = sd8200model->index(counter, 11); // 第11列的索引
                            QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                            QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                            sd8200model->setData(index, finalString);
                            counter += 2; // 每隔一个单元格设置数据

                            for (int j = 0; j < 8; j++) {
                                bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                QString binaryString = bit ? "1" : "0";
                                sd8200model->setData(sd8200model->index(row, currentCol), binaryString);
                                currentCol++;
                                if (currentCol > endCol) {
                                    currentCol = startCol;
                                    row+=2;//隔一行存放数据
                                }
                            }
                        }
                        QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                        qDebug() << "读10H："<<ROOdata.size();
                    }
                }
            });

        }else if (text=="SD8804") {
            sd8804Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x86);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                    if(ROOdata.size()==110)
                  {
                        // 解析并设置BCD码数据
                        int row = 80;
                        int counter = 80;//行计数
                        int startCol = 3; // 定义数据存放的起始列
                        int endCol = 10; // 定义数据存放的结束列
                        int currentCol = startCol; // 当前处理的列从起始列开始
                        for (int i = 0; i < ROOdata.size() && i<109; i++) {
                            QModelIndex index = sd8804model->index(counter, 11); // 第11列的索引
                            QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                            QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                            sd8804model->setData(index, finalString);
                            counter += 2; // 每隔一个单元格设置数据

                            for (int j = 0; j < 8; j++) {
                                bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                QString binaryString = bit ? "1" : "0";
                                sd8804model->setData(sd8804model->index(row, currentCol), binaryString);
                                currentCol++;
                                if (currentCol > endCol) {
                                    currentCol = startCol;
                                    row+=2;//隔一行存放数据
                                }
                            }
                        }
                        QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                        qDebug() << "读10H："<<ROOdata.size();
                    }
                }
            });

        }else if (text=="SD8840") {
            sd8840Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x4A);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                    if(ROOdata.size()==20)
                  {
                        // 解析并设置BCD码数据
                        int row = 51;
                        int col = 0;
                        int counter = 51;//行计数
                        for (int i = 0; i < ROOdata.size() && i<19; i++) {

                            QModelIndex index = sd8840model->index(counter, 8); // 第8列的索引
                            QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                            QString finalString = hexString + "H"; // 在16进制数据添加字符"H"

                            sd8840model->setData(index, finalString);

                            counter += 2; // 每隔一个单元格设置数据

                            for (int j = 0; j < 8; j++) {
                                bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                QString binaryString = bit ? "1" : "0";
                                sd8840model->setData(sd8840model->index(row, col), binaryString);
                                col++;
                                if (col >= 8) {
                                    col = 0;
                                    row+=2;//隔一行存放数据
                                }
                            }
                        }
                        QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                        qDebug() << "读10H："<<ROOdata.size();
                    }
                }
            });

        }else if (text=="SD8902") {
            sd8902Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x4C);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                    if(ROOdata.size()==9)
                  {
                        // 解析并设置BCD码数据
                        int row = 35;
                        int counter = 35;//行计数
                        int startCol = 3; // 定义数据存放的起始列
                        int endCol = 10; // 定义数据存放的结束列
                        int currentCol = startCol; // 当前处理的列从起始列开始
                        for (int i = 0; i < ROOdata.size() && i<8; i++) {
                            QModelIndex index = sd8902model->index(counter, 11); // 第11列的索引
                            QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                            QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                            sd8902model->setData(index, finalString);
                            counter += 2; // 每隔一个单元格设置数据

                            for (int j = 0; j < 8; j++) {
                                bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                QString binaryString = bit ? "1" : "0";
                                sd8902model->setData(sd8902model->index(row, currentCol), binaryString);
                                currentCol++;
                                if (currentCol > endCol) {
                                    currentCol = startCol;
                                    row+=2;//隔一行存放数据
                                }
                            }
                        }
                        QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                        qDebug() << "读10H："<<ROOdata.size();
                    }
                }
            });

        }

       }else{
        QMessageBox::information(this,"错误","串口打开失败，串口未连接或者被占用!");
    }
}

/**
功能：读5按键--读取芯片的寄存器，具体寄存器根据具体芯片型号来确定
修改：sjm
参数：-
返回值：-
**/
void RtcMessage::on_R60H_clicked()
{
    QString text = ui->rtctype->toPlainText();
    //清空输入缓冲区
    serialPort->clear(QSerialPort::Input);

    if(serialPort->isOpen())
    {
        if(text=="SD3068"||text=="SD3010"||text=="SD3025"||text=="SD3031"||text=="SD3077"||text=="SD3078"||text=="SD3178"||text=="SD3900"||text=="SD2503"||text=="SD2505"||text=="SD2506"||text=="SD2507"||text=="SD2508"||text=="SD2520"||text=="SD2522"||text=="SD2523")
        {
             sd3068Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0xA7);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray R60data=serialPort->readAll();
                qDebug()<<"收到数据："<<R60data;
                qDebug() <<R60data.size();

                if (!R60data.isEmpty())
                {
                    if(R60data.size()==27)
                    {
                        // 解析并设置BCD码数据
                            int row = 128;
                            int counter = 128;//行计数
                            int startCol = 3; // 定义数据存放的起始列
                            int endCol = 10; // 定义数据存放的结束列
                            int currentCol = startCol; // 当前处理的列从起始列开始
                            for (int i = 0; i < R60data.size() && i < 26; i++) {
                                QModelIndex index = sd3068model->index(counter, 11); // 第11列的索引
                                QString hexString = QString::number(static_cast<quint8>(R60data[i]), 16).toUpper();
                                QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                                sd3068model->setData(index, finalString);
                                counter++;

                                for (int j = 0; j < 8; j++) {
                                    bool bit = (R60data.at(i) >> (7 - j)) & 1;
                                    QString binaryString = bit ? "1" : "0";
                                    sd3068model->setData(sd3068model->index(row, currentCol), binaryString);
                                    currentCol++;
                                    if (currentCol > endCol) {
                                        currentCol = startCol;
                                        row++;
                                    }
                                }
                            }

                            QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                            qDebug() << "读寄存器60H："<<R60data.size();
                    }
                }
            });

        }else if (text=="SD8810") {
            sd8810Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x22);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray R20data=serialPort->readAll();
                qDebug()<<"收到数据："<<R20data;
                qDebug() <<R20data.size();

                if (!R20data.isEmpty())
                {
                    if(R20data.size()==5)
                    {
                        // 解析并设置BCD码数据
                            int row = 199;
                            int counter = 199;//行计数
                            int startCol = 3; // 定义数据存放的起始列
                            int endCol = 10; // 定义数据存放的结束列
                            int currentCol = startCol; // 当前处理的列从起始列开始
                            for (int i = 0; i < R20data.size() && i < 4; i++) {
                                QModelIndex index = sd8810model->index(counter, 11); // 第11列的索引
                                QString hexString = QString::number(static_cast<quint8>(R20data[i]), 16).toUpper();
                                QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                                sd8810model->setData(index, finalString);
                                counter +=2;

                                for (int j = 0; j < 8; j++) {
                                    bool bit = (R20data.at(i) >> (7 - j)) & 1;
                                    QString binaryString = bit ? "1" : "0";
                                    sd8810model->setData(sd8810model->index(row, currentCol), binaryString);
                                    currentCol++;
                                    if (currentCol > endCol) {
                                        currentCol = startCol;
                                        row+=2;//隔一行存放数据
                                    }
                                }
                            }
                            QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                            qDebug() << "读20H："<<R20data.size();
                    }
                }
            });

        }else if (text=="SD8800") {
            sd8800Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x44);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                    if(ROOdata.size()==14)
                  {
                        // 解析并设置BCD码数据
                        int row = 166;
                        int counter = 166;//行计数
                        int startCol = 3; // 定义数据存放的起始列
                        int endCol = 10; // 定义数据存放的结束列
                        int currentCol = startCol; // 当前处理的列从起始列开始
                        for (int i = 0; i < ROOdata.size() && i<13; i++) {
                            QModelIndex index = sd8800model->index(counter, 11); // 第11列的索引
                            QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                            QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                            sd8800model->setData(index, finalString);
                            counter += 2; // 每隔一个单元格设置数据

                            for (int j = 0; j < 8; j++) {
                                bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                QString binaryString = bit ? "1" : "0";
                                sd8800model->setData(sd8800model->index(row, currentCol), binaryString);
                                currentCol++;
                                if (currentCol > endCol) {
                                    currentCol = startCol;
                                    row+=2;//隔一行存放数据
                                }
                            }
                        }
                        QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                        qDebug() << "读10H："<<ROOdata.size();
                    }
                }
            });

        }else if (text=="SD8902") {
            sd8902Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x4D);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                    if(ROOdata.size()==32)
                  {
                        // 解析并设置BCD码数据
                        int row = 51;
                        int counter = 51;//行计数
                        int startCol = 3; // 定义数据存放的起始列
                        int endCol = 10; // 定义数据存放的结束列
                        int currentCol = startCol; // 当前处理的列从起始列开始
                        for (int i = 0; i < ROOdata.size() && i<31; i++) {
                            QModelIndex index = sd8902model->index(counter, 11); // 第11列的索引
                            QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                            QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                            sd8902model->setData(index, finalString);
                            counter += 2; // 每隔一个单元格设置数据

                            for (int j = 0; j < 8; j++) {
                                bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                QString binaryString = bit ? "1" : "0";
                                sd8902model->setData(sd8902model->index(row, currentCol), binaryString);
                                currentCol++;
                                if (currentCol > endCol) {
                                    currentCol = startCol;
                                    row+=2;//隔一行存放数据
                                }
                            }
                        }
                        QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                        qDebug() << "读10H："<<ROOdata.size();
                    }
                }
            });

        }else if (text=="SD8804") {
            sd8804Modelselt();
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x4D);
            serialPort->write(hexData);

            QTimer::singleShot(100, [&](){
                QByteArray ROOdata=serialPort->readAll();
                qDebug()<<"收到数据："<<ROOdata;
                qDebug() <<ROOdata.size();

                if (!ROOdata.isEmpty())
                {
                    if(ROOdata.size()==5)
                  {
                        // 解析并设置BCD码数据
                        int row = 190;
                        int counter = 190;//行计数
                        int startCol = 3; // 定义数据存放的起始列
                        int endCol = 10; // 定义数据存放的结束列
                        int currentCol = startCol; // 当前处理的列从起始列开始
                        for (int i = 0; i < ROOdata.size() && i<4; i++) {
                            QModelIndex index = sd8804model->index(counter, 11); // 第11列的索引
                            QString hexString = QString::number(static_cast<quint8>(ROOdata[i]), 16).toUpper();
                            QString finalString = hexString + "H"; // 在16进制数据添加字符"H"
                            sd8804model->setData(index, finalString);
                            counter += 2; // 每隔一个单元格设置数据

                            for (int j = 0; j < 8; j++) {
                                bool bit = (ROOdata.at(i) >> (7 - j)) & 1;
                                QString binaryString = bit ? "1" : "0";
                                sd8804model->setData(sd8804model->index(row, currentCol), binaryString);
                                currentCol++;
                                if (currentCol > endCol) {
                                    currentCol = startCol;
                                    row+=2;//隔一行存放数据
                                }
                            }
                        }
                        QMessageBox::information(this, "读寄存器", "读取寄存器成功！");
                        qDebug() << "读10H："<<ROOdata.size();
                    }
                }
            });
        }

        }else{

        QMessageBox::information(this,"错误","串口打开失败，串口未连接或者被占用!");

    }
}

/*********************************************************写寄存器槽函数实现***************************************************************/
/**
功能：写1-1按键--写当前时间给芯片
修改：sjm
参数：-
返回值：-
**/
void RtcMessage::on_writetime_clicked()
{
    QString text = ui->rtctype->toPlainText();

    if(serialPort->isOpen())
    {
        // 获取当前日期和时间
        QDateTime currentDateTime = QDateTime::currentDateTime();

        // 拆分日期和时间
        int year = currentDateTime.date().year();
        int month = currentDateTime.date().month();
        int day = currentDateTime.date().day();
        int hour = currentDateTime.time().hour();
        int minute = currentDateTime.time().minute();
        int second = currentDateTime.time().second();
        int weekday = currentDateTime.date().dayOfWeek();

        if(text=="SD3068"||text=="SD3010"||text=="SD3025"||text=="SD3031"||text=="SD3077"||text=="SD3078"||text=="SD3178"||text=="SD3900"||text=="SD2503"||text=="SD2505"||text=="SD2506"||text=="SD2507"||text=="SD2508"||text=="SD2520"||text=="SD2522"||text=="SD2523")
        {
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x80);
            serialPort->write(hexData);

            // 将时间字段转换为BCD码并发送到单片机
            QByteArray bcdData;
            bcdData.append(static_cast<char>(((second / 10) << 4) | (second % 10)));  // 秒
            bcdData.append(static_cast<char>(((minute / 10) << 4) | (minute % 10)));  // 分钟
            bcdData.append(static_cast<char>(((hour / 10) << 4) | (hour % 10))|0x80);      // 小时
            bcdData.append(static_cast<char>(weekday)); // 星期
            bcdData.append(static_cast<char>(((day / 10) << 4) | (day % 10)));        // 日
            bcdData.append(static_cast<char>(((month / 10) << 4) | (month % 10)));    // 月
            bcdData.append(static_cast<char>(((year % 100 / 10) << 4) | (year % 100 % 10)));  // 年的后两位
            serialPort->write(bcdData);

        }else if (text=="SD8563"||text=="SD8564"||text=="SD8565"||text=="SD8568") {

            // 将时间字段转换为BCD码并发送到单片机
            QByteArray bcdData;

            bcdData.append(static_cast<char>(((second / 10) << 4) | (second % 10)));  // 秒
            bcdData.append(static_cast<char>(((minute / 10) << 4) | (minute % 10)));  // 分钟
            bcdData.append(static_cast<char>(((hour / 10) << 4) | (hour % 10))|0x80);      // 小时
            bcdData.append(static_cast<char>(((day / 10) << 4) | (day % 10)));        // 日
            bcdData.append(static_cast<char>(weekday)); // 星期
            bcdData.append(static_cast<char>(((month / 10) << 4) | (month % 10)));    // 月
            bcdData.append(static_cast<char>(((year % 100 / 10) << 4) | (year % 100 % 10)));  // 年的后两位

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x11);

            serialPort->write(hexData);
            serialPort->write(bcdData);

        }else if (text=="SD8810") {

            // 将时间字段转换为BCD码并发送到单片机
            QByteArray bcdData;

            bcdData.append(static_cast<char>(((second / 10) << 4) | (second % 10)));  // 秒
            bcdData.append(static_cast<char>(((minute / 10) << 4) | (minute % 10)));  // 分钟
            bcdData.append(static_cast<char>(((hour / 10) << 4) | (hour % 10)));      // 小时
            bcdData.append(static_cast<char>(weekday)); // 星期
            bcdData.append(static_cast<char>(((day / 10) << 4) | (day % 10)));        // 日
            bcdData.append(static_cast<char>(((month / 10) << 4) | (month % 10)));    // 月
            bcdData.append(static_cast<char>(((year % 100 / 10) << 4) | (year % 100 % 10)));  // 年的后两位

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x15);
            serialPort->write(hexData);
            serialPort->write(bcdData);
        }else if (text=="SD8939") {

            // 将时间字段转换为BCD码并发送到单片机
            QByteArray bcdData;

            bcdData.append(static_cast<char>(((second / 10) << 4) | (second % 10)));  // 秒
            bcdData.append(static_cast<char>(((minute / 10) << 4) | (minute % 10)));  // 分钟
            bcdData.append(static_cast<char>(((hour / 10) << 4) | (hour % 10)));      // 小时
            bcdData.append(static_cast<char>(weekday)); // 星期
            bcdData.append(static_cast<char>(((day / 10) << 4) | (day % 10)));        // 日
            bcdData.append(static_cast<char>(((month / 10) << 4) | (month % 10)));    // 月
            bcdData.append(static_cast<char>(((year % 100 / 10) << 4) | (year % 100 % 10)));  // 年的后两位

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x27);
            serialPort->write(hexData);
            serialPort->write(bcdData);
        }else if (text=="SD8800") {
            // 将时间字段转换为BCD码并发送到单片机
            QByteArray bcdData;

            bcdData.append(static_cast<char>(((second / 10) << 4) | (second % 10)));  // 秒
            bcdData.append(static_cast<char>(((minute / 10) << 4) | (minute % 10)));  // 分钟
            bcdData.append(static_cast<char>(((hour / 10) << 4) | (hour % 10)));      // 小时
            bcdData.append(static_cast<char>(weekday)); // 星期
            bcdData.append(static_cast<char>(((day / 10) << 4) | (day % 10)));        // 日
            bcdData.append(static_cast<char>(((month / 10) << 4) | (month % 10)));    // 月
            bcdData.append(static_cast<char>(((year % 100 / 10) << 4) | (year % 100 % 10)));  // 年的后两位

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x37);
            serialPort->write(hexData);
            serialPort->write(bcdData);
        }else if (text=="SD8931") {
            // 将时间字段转换为BCD码并发送到单片机
            QByteArray bcdData;

            bcdData.append(static_cast<char>(((second / 10) << 4) | (second % 10)));  // 秒
            bcdData.append(static_cast<char>(((minute / 10) << 4) | (minute % 10)));  // 分钟
            bcdData.append(static_cast<char>(((hour / 10) << 4) | (hour % 10)));      // 小时
            bcdData.append(static_cast<char>(weekday)); // 星期
            bcdData.append(static_cast<char>(((day / 10) << 4) | (day % 10)));        // 日
            bcdData.append(static_cast<char>(((month / 10) << 4) | (month % 10)));    // 月
            bcdData.append(static_cast<char>(((year % 100 / 10) << 4) | (year % 100 % 10)));  // 年的后两位

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x49);
            serialPort->write(hexData);
            serialPort->write(bcdData);
        }else if (text=="SD8825") {
            // 将时间字段转换为BCD码并发送到单片机
            QByteArray bcdData;

            bcdData.append(static_cast<char>(((second / 10) << 4) | (second % 10)));  // 秒
            bcdData.append(static_cast<char>(((minute / 10) << 4) | (minute % 10)));  // 分钟
            bcdData.append(static_cast<char>(((hour / 10) << 4) | (hour % 10)));      // 小时
            bcdData.append(static_cast<char>(weekday)); // 星期
            bcdData.append(static_cast<char>(((day / 10) << 4) | (day % 10)));        // 日
            bcdData.append(static_cast<char>(((month / 10) << 4) | (month % 10)));    // 月
            bcdData.append(static_cast<char>(((year % 100 / 10) << 4) | (year % 100 % 10)));  // 年的后两位

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x59);
            serialPort->write(hexData);
            serialPort->write(bcdData);
        }else if (text=="SD8200") {
            // 将时间字段转换为BCD码并发送到单片机
            QByteArray bcdData;

            bcdData.append(static_cast<char>(((second / 10) << 4) | (second % 10)));  // 秒
            bcdData.append(static_cast<char>(((minute / 10) << 4) | (minute % 10)));  // 分钟
            bcdData.append(static_cast<char>(((hour / 10) << 4) | (hour % 10)));      // 小时
            bcdData.append(static_cast<char>(weekday)); // 星期
            bcdData.append(static_cast<char>(((day / 10) << 4) | (day % 10)));        // 日
            bcdData.append(static_cast<char>(((month / 10) << 4) | (month % 10)));    // 月
            bcdData.append(static_cast<char>(((year % 100 / 10) << 4) | (year % 100 % 10)));  // 年的后两位

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x69);
            serialPort->write(hexData);
            serialPort->write(bcdData);
        }else if (text=="SD8804") {
            // 将时间字段转换为BCD码并发送到单片机
            QByteArray bcdData;

            bcdData.append(static_cast<char>(((second / 10) << 4) | (second % 10)));  // 秒
            bcdData.append(static_cast<char>(((minute / 10) << 4) | (minute % 10)));  // 分钟
            bcdData.append(static_cast<char>(((hour / 10) << 4) | (hour % 10)));      // 小时
            bcdData.append(static_cast<char>(weekday)); // 星期
            bcdData.append(static_cast<char>(((day / 10) << 4) | (day % 10)));        // 日
            bcdData.append(static_cast<char>(((month / 10) << 4) | (month % 10)));    // 月
            bcdData.append(static_cast<char>(((year % 100 / 10) << 4) | (year % 100 % 10)));  // 年的后两位

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x79);
            serialPort->write(hexData);
            serialPort->write(bcdData);
        }else if (text=="SD8902") {
            // 将时间字段转换为BCD码并发送到单片机
            QByteArray bcdData;

            bcdData.append(static_cast<char>(((second / 10) << 4) | (second % 10)));  // 秒
            bcdData.append(static_cast<char>(((minute / 10) << 4) | (minute % 10)));  // 分钟
            bcdData.append(static_cast<char>(((hour / 10) << 4) | (hour % 10)));      // 小时
            bcdData.append(static_cast<char>(((day / 10) << 4) | (day % 10)));        // 日
            bcdData.append(static_cast<char>(((month / 10) << 4) | (month % 10)));    // 月
            bcdData.append(static_cast<char>(weekday)); // 星期
            bcdData.append(static_cast<char>(((year % 100 / 10) << 4) | (year % 100 % 10)));  // 年的后两位

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x99);
            serialPort->write(hexData);
            serialPort->write(bcdData);
        }else if (text=="SD2010") {
            // 将时间字段转换为BCD码并发送到单片机
            QByteArray bcdData;

            bcdData.append(static_cast<char>(((second / 10) << 4) | (second % 10)));  // 秒
            bcdData.append(static_cast<char>(((minute / 10) << 4) | (minute % 10)));  // 分钟
            bcdData.append(static_cast<char>(((hour / 10) << 4) | (hour % 10))|0x80);      // 小时
            bcdData.append(static_cast<char>(weekday)); // 星期
            bcdData.append(static_cast<char>(((day / 10) << 4) | (day % 10)));        // 日
            bcdData.append(static_cast<char>(((month / 10) << 4) | (month % 10)));    // 月
            bcdData.append(static_cast<char>(((year % 100 / 10) << 4) | (year % 100 % 10)));  // 年的后两位

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            uint16_t value = 2;
            QByteArray valueBytes(reinterpret_cast<const char*>(&value), sizeof(value));
            hexData.append(valueBytes);

            serialPort->write(hexData);
            serialPort->write(bcdData);

        }else if (text=="SD2058"||text=="SD2059"||text=="SD2068"||text=="SD2069"||text=="SD2077"||text=="SD2078"||text=="SD2400"||text=="SD2401"||text=="SD2402"||text=="SD2403"||text=="SD2404"||text=="SD2405") {
            // 将时间字段转换为BCD码并发送到单片机
            QByteArray bcdData;

            bcdData.append(static_cast<char>(((second / 10) << 4) | (second % 10)));  // 秒
            bcdData.append(static_cast<char>(((minute / 10) << 4) | (minute % 10)));  // 分钟
            bcdData.append(static_cast<char>(((hour / 10) << 4) | (hour % 10))|0x80);      // 小时
            bcdData.append(static_cast<char>(weekday)); // 星期
            bcdData.append(static_cast<char>(((day / 10) << 4) | (day % 10)));        // 日
            bcdData.append(static_cast<char>(((month / 10) << 4) | (month % 10)));    // 月
            bcdData.append(static_cast<char>(((year % 100 / 10) << 4) | (year % 100 % 10)));  // 年的后两位

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x80);

            serialPort->write(hexData);
            serialPort->write(bcdData);
        }else if (text=="SD8840") {
            // 将时间字段转换为BCD码并发送到单片机
            QByteArray bcdData;

            bcdData.append(static_cast<char>(((second / 10) << 4) | (second % 10)));  // 秒
            bcdData.append(static_cast<char>(((minute / 10) << 4) | (minute % 10)));  // 分钟
            bcdData.append(static_cast<char>(((hour / 10) << 4) | (hour % 10))|0x80);      // 小时
            bcdData.append(static_cast<char>(weekday)); // 星期
            bcdData.append(static_cast<char>(((day / 10) << 4) | (day % 10)));        // 日
            bcdData.append(static_cast<char>(((month / 10) << 4) | (month % 10)));    // 月
            bcdData.append(static_cast<char>(((year % 100 / 10) << 4) | (year % 100 % 10)));  // 年的后两位

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x89);

            serialPort->write(hexData);
            serialPort->write(bcdData);
        }else if (text=="SD2200"||text=="SD2201"||text=="SD2202"||text=="SD2203"||text=="SD2204")
        {
            // 将时间字段转换为BCD码并发送到单片机
            QByteArray bcdData;

            bcdData.append(static_cast<char>(((year % 100 / 10) << 4) | (year % 100 % 10)));  // 年的后两位
            bcdData.append(static_cast<char>(((month / 10) << 4) | (month % 10)));    // 月
            bcdData.append(static_cast<char>(((day / 10) << 4) | (day % 10)));        // 日
            bcdData.append(static_cast<char>(weekday)); // 星期
            bcdData.append(static_cast<char>(((hour / 10) << 4) | (hour % 10))|0x80);      // 小时
            bcdData.append(static_cast<char>(((minute / 10) << 4) | (minute % 10)));  // 分钟
            bcdData.append(static_cast<char>(((second / 10) << 4) | (second % 10)));  // 秒

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            uint16_t value = 9;
            QByteArray valueBytes(reinterpret_cast<const char*>(&value), sizeof(value));
            hexData.append(valueBytes);

            serialPort->write(hexData);
            serialPort->write(bcdData);
        }

        QMessageBox::information(this, "写时间", "时间写入成功！");

    }else{

        QMessageBox::information(this,"错误","串口打开失败，串口未连接或者被占用!");

    }

}

/**
功能：写2-2按键--写芯片的时间寄存器，具体寄存器根据具体芯片型号来确定
修改：sjm
参数：-
返回值：-
**/
void RtcMessage::on_W00H_clicked()
{
    QString text = ui->rtctype->toPlainText();

    if(serialPort->isOpen())
    {
        if(text=="SD3068"||text=="SD3010"||text=="SD3025"||text=="SD3031"||text=="SD3077"||text=="SD3078"||text=="SD3178"||text=="SD3900"||text=="SD2503"||text=="SD2505"||text=="SD2506"||text=="SD2507"||text=="SD2508"||text=="SD2520"||text=="SD2522"||text=="SD2523")
        {
            sd3068Modelselt();
            QString binaryData;
                    for (int row = 1; row <= 13; row += 2)//row = 1(数据库行号，行从0开始索引)，Qtableview显示行从1开始，看Qtableview显示，找到需要的行号，行-1即为实际数据库的行号，
                    {
                        for (int col = 3; col < 11; col++)//获取3~11列的数据
                        {
                            QString binaryValue = sd3068model->data(sd3068model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

            // 判断特定单元格的值是否为1--写保护置位
            if (sd3068model->data(sd3068model->index(31, 3)).toString() == "1" &&
                sd3068model->data(sd3068model->index(31, 8)).toString() == "1" &&
                sd3068model->data(sd3068model->index(33, 3)).toString() == "1"){
               // 将二进制数据转换为BCD码并发送给单片机
               QByteArray bcdData;
               for (int i = 0; i < binaryData.length(); i += 8)
               {
                   QString binary = binaryData.mid(i, 8);
                   bool ok;
                   int decimal = binary.toInt(&ok, 2);
                   if (ok)
                   {
                       QString bcd = QString::number(decimal, 16);
                       bcdData.append(bcd.toInt(nullptr, 16));
                   }
               }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x80);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
            }else{

                QMessageBox::information(this, "写寄存器", "写允许未打开：WRTC1,WRTC2,WRTC3未置1!");

            }

       }else if (text=="SD8563"||text=="SD8564"||text=="SD8565"||text=="SD8568") {
            sd8563Modelselt();
            QString binaryData;
            for (int row = 5; row <= 17; row += 2)
            {
                for (int col = 3; col < 11; col++)
                {
                    QString binaryValue = sd8563model->data(sd8563model->index(row, col)).toString();
                    binaryData += binaryValue;
                }

             }
            // 将二进制数据转换为BCD码并发送给单片机
           QByteArray bcdData;
           for (int i = 0; i < binaryData.length(); i += 8)
           {
               QString binary = binaryData.mid(i, 8);
               bool ok;
               int decimal = binary.toInt(&ok, 2);
               if (ok)
               {
                   QString bcd = QString::number(decimal, 16);
                   bcdData.append(bcd.toInt(nullptr, 16));
               }
           }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x11);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();

       }else if (text=="SD8810") {
            sd8810Modelselt();
            QString binaryData;
                    for (int row = 1; row <= 13; row += 2)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd8810model->data(sd8810model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                   QByteArray bcdData;
                   for (int i = 0; i < binaryData.length(); i += 8)
                   {
                       QString binary = binaryData.mid(i, 8);
                       bool ok;
                       int decimal = binary.toInt(&ok, 2);
                       if (ok)
                       {
                           QString bcd = QString::number(decimal, 16);
                           bcdData.append(bcd.toInt(nullptr, 16));
                       }
                   }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x17);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
       }else if (text=="SD8939") {
            sd8939Modelselt();
            QString binaryData;
                    for (int row = 1; row <= 13; row += 2)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd8939model->data(sd8939model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x29);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
       }else if (text=="SD8800") {
            sd8800Modelselt();
            QString binaryData;
                    for (int row = 1; row <= 13; row += 2)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd8800model->data(sd8800model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x39);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
       }else if (text=="SD8931") {
            sd8931Modelselt();
            QString binaryData;
                    for (int row = 1; row <= 13; row += 2)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd8931model->data(sd8931model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x51);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
       }else if (text=="SD8825") {
            sd8825Modelselt();
            QString binaryData;
                    for (int row = 1; row <= 13; row += 2)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd8825model->data(sd8825model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x61);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
       }else if (text=="SD8200") {
            sd8200Modelselt();
            QString binaryData;
                    for (int row = 1; row <= 13; row += 2)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd8200model->data(sd8200model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x71);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
       }else if (text=="SD8804") {
            sd8804Modelselt();
            QString binaryData;
                    for (int row = 1; row <= 13; row += 2)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd8804model->data(sd8804model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x3F);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
       }else if (text=="SD8902") {
            sd8902Modelselt();
            QString binaryData;
                    for (int row = 1; row <= 13; row += 2)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd8902model->data(sd8902model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0xA1);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
       }else if (text=="SD2010") {
            sd2010Modelselt();
            QString binaryData;
            for (int row = 1; row <= 13; row +=2)
            {
                for (int col = 3; col < 11; col++)
                {
                    QString binaryValue = sd2010model->data(sd2010model->index(row, col)).toString();
                    binaryData += binaryValue;
                }
            }

            // 判断特定单元格的值是否为1
            if (sd2010model->data(sd2010model->index(31, 3)).toString() == "1" &&
                sd2010model->data(sd2010model->index(31, 8)).toString() == "1" &&
                sd2010model->data(sd2010model->index(33, 3)).toString() == "1"){
                // 将二进制数据转换为BCD码并发送给单片机
                   QByteArray bcdData;
                   for (int i = 0; i < binaryData.length(); i += 8)
                   {
                       QString binary = binaryData.mid(i, 8);
                       bool ok;
                       int decimal = binary.toInt(&ok, 2);
                       if (ok)
                       {
                           QString bcd = QString::number(decimal, 16);
                           bcdData.append(bcd.toInt(nullptr, 16));
                       }
                   }

                   QByteArray hexData;
                   hexData.append(0xBC);
                   hexData.append(0xCD);
                   uint16_t value = 4;
                   QByteArray valueBytes(reinterpret_cast<const char*>(&value), sizeof(value));
                   hexData.append(valueBytes);

                   serialPort->write(hexData);
                   serialPort->write(bcdData);

                   QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
                   qDebug()<<"写入数据："<<bcdData;
                   qDebug()<<"写入数据："<<bcdData.size();

            }else{

                QMessageBox::information(this, "写寄存器", "写允许未打开：WRTC1,WRTC2,WRTC3未置1!");

            }

       }else if (text=="SD2058") {
            sd2058Modelselt();
            QString binaryData;
            for (int row = 1; row <= 13; row +=2)
            {
                for (int col = 3; col < 11; col++)
                {
                    QString binaryValue = sd2058model->data(sd2058model->index(row, col)).toString();
                    binaryData += binaryValue;
                }
            }

            // 判断特定单元格的值是否为1
            if (sd2058model->data(sd2058model->index(31, 3)).toString() == "1" &&
                sd2058model->data(sd2058model->index(31, 8)).toString() == "1" &&
                sd2058model->data(sd2058model->index(33, 3)).toString() == "1"){
                // 将二进制数据转换为BCD码并发送给单片机
                   QByteArray bcdData;
                   for (int i = 0; i < binaryData.length(); i += 8)
                   {
                       QString binary = binaryData.mid(i, 8);
                       bool ok;
                       int decimal = binary.toInt(&ok, 2);
                       if (ok)
                       {
                           QString bcd = QString::number(decimal, 16);
                           bcdData.append(bcd.toInt(nullptr, 16));
                       }
                   }

                   QByteArray hexData;
                   hexData.append(0xBC);
                   hexData.append(0xCD);
                   uint16_t value = 4;
                   QByteArray valueBytes(reinterpret_cast<const char*>(&value), sizeof(value));
                   hexData.append(valueBytes);

                   serialPort->write(hexData);
                   serialPort->write(bcdData);

                   QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
                   qDebug()<<"写入数据："<<bcdData;
                   qDebug()<<"写入数据："<<bcdData.size();

            }else{

                QMessageBox::information(this, "写寄存器", "写允许未打开：WRTC1,WRTC2,WRTC3未置1!");

            }
       }else if (text=="SD2059") {
            sd2059Modelselt();
            QString binaryData;
            for (int row = 1; row <= 13; row +=2)
            {
                for (int col = 3; col < 11; col++)
                {
                    QString binaryValue = sd2059model->data(sd2059model->index(row, col)).toString();
                    binaryData += binaryValue;
                }
            }

            // 判断特定单元格的值是否为1
            if (sd2059model->data(sd2059model->index(31, 3)).toString() == "1" &&
                sd2059model->data(sd2059model->index(31, 8)).toString() == "1" &&
                sd2059model->data(sd2059model->index(33, 3)).toString() == "1"){
                // 将二进制数据转换为BCD码并发送给单片机
                   QByteArray bcdData;
                   for (int i = 0; i < binaryData.length(); i += 8)
                   {
                       QString binary = binaryData.mid(i, 8);
                       bool ok;
                       int decimal = binary.toInt(&ok, 2);
                       if (ok)
                       {
                           QString bcd = QString::number(decimal, 16);
                           bcdData.append(bcd.toInt(nullptr, 16));
                       }
                   }

                   QByteArray hexData;
                   hexData.append(0xBC);
                   hexData.append(0xCD);
                   uint16_t value = 4;
                   QByteArray valueBytes(reinterpret_cast<const char*>(&value), sizeof(value));
                   hexData.append(valueBytes);

                   serialPort->write(hexData);
                   serialPort->write(bcdData);

                   QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
                   qDebug()<<"写入数据："<<bcdData;
                   qDebug()<<"写入数据："<<bcdData.size();

            }else{

                QMessageBox::information(this, "写寄存器", "写允许未打开：WRTC1,WRTC2,WRTC3未置1!");

            }
       }else if (text=="SD2068"||text=="SD2400"||text=="SD2401"||text=="SD2402"||text=="SD2403"||text=="SD2404"||text=="SD2405") {
            sd2068Modelselt();
            QString binaryData;
            for (int row = 1; row <= 13; row +=2)
            {
                for (int col = 3; col < 11; col++)
                {
                    QString binaryValue = sd2068model->data(sd2068model->index(row, col)).toString();
                    binaryData += binaryValue;
                }
            }

            // 判断特定单元格的值是否为1
            if (sd2068model->data(sd2068model->index(31, 0)).toString() == "1" &&
                sd2068model->data(sd2068model->index(31, 5)).toString() == "1" &&
                sd2068model->data(sd2068model->index(33, 0)).toString() == "1"){
                // 将二进制数据转换为BCD码并发送给单片机
                   QByteArray bcdData;
                   for (int i = 0; i < binaryData.length(); i += 8)
                   {
                       QString binary = binaryData.mid(i, 8);
                       bool ok;
                       int decimal = binary.toInt(&ok, 2);
                       if (ok)
                       {
                           QString bcd = QString::number(decimal, 16);
                           bcdData.append(bcd.toInt(nullptr, 16));
                       }
                   }

                   QByteArray hexData;
                   hexData.append(0xBC);
                   hexData.append(0xCD);
                   uint16_t value = 4;
                   QByteArray valueBytes(reinterpret_cast<const char*>(&value), sizeof(value));
                   hexData.append(valueBytes);

                   serialPort->write(hexData);
                   serialPort->write(bcdData);

                   QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
                   qDebug()<<"写入数据："<<bcdData;
                   qDebug()<<"写入数据："<<bcdData.size();

            }else{

                QMessageBox::information(this, "写寄存器", "写允许未打开：WRTC1,WRTC2,WRTC3未置1!");

            }
       }else if (text=="SD2069") {
            sd2069Modelselt();
            QString binaryData;
            for (int row = 1; row <= 13; row +=2)
            {
                for (int col = 3; col < 11; col++)
                {
                    QString binaryValue = sd2069model->data(sd2069model->index(row, col)).toString();
                    binaryData += binaryValue;
                }
            }

            // 判断特定单元格的值是否为1
            if (sd2069model->data(sd2069model->index(31, 3)).toString() == "1" &&
                sd2069model->data(sd2069model->index(31, 8)).toString() == "1" &&
                sd2069model->data(sd2069model->index(33, 3)).toString() == "1"){
                // 将二进制数据转换为BCD码并发送给单片机
                   QByteArray bcdData;
                   for (int i = 0; i < binaryData.length(); i += 8)
                   {
                       QString binary = binaryData.mid(i, 8);
                       bool ok;
                       int decimal = binary.toInt(&ok, 2);
                       if (ok)
                       {
                           QString bcd = QString::number(decimal, 16);
                           bcdData.append(bcd.toInt(nullptr, 16));
                       }
                   }

                   QByteArray hexData;
                   hexData.append(0xBC);
                   hexData.append(0xCD);
                   uint16_t value = 4;
                   QByteArray valueBytes(reinterpret_cast<const char*>(&value), sizeof(value));
                   hexData.append(valueBytes);

                   serialPort->write(hexData);
                   serialPort->write(bcdData);

                   QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
                   qDebug()<<"写入数据："<<bcdData;
                   qDebug()<<"写入数据："<<bcdData.size();

            }else{

                QMessageBox::information(this, "写寄存器", "写允许未打开：WRTC1,WRTC2,WRTC3未置1!");

            }
       }else if (text=="SD2077") {
            sd2077Modelselt();
            QString binaryData;
            for (int row = 1; row <= 13; row +=2)
            {
                for (int col = 3; col < 11; col++)
                {
                    QString binaryValue = sd2077model->data(sd2077model->index(row, col)).toString();
                    binaryData += binaryValue;
                }
            }

            // 判断特定单元格的值是否为1
            if (sd2077model->data(sd2077model->index(31, 3)).toString() == "1" &&
                sd2077model->data(sd2077model->index(31, 8)).toString() == "1" &&
                sd2077model->data(sd2077model->index(33, 3)).toString() == "1"){
                // 将二进制数据转换为BCD码并发送给单片机
                   QByteArray bcdData;
                   for (int i = 0; i < binaryData.length(); i += 8)
                   {
                       QString binary = binaryData.mid(i, 8);
                       bool ok;
                       int decimal = binary.toInt(&ok, 2);
                       if (ok)
                       {
                           QString bcd = QString::number(decimal, 16);
                           bcdData.append(bcd.toInt(nullptr, 16));
                       }
                   }

                   QByteArray hexData;
                   hexData.append(0xBC);
                   hexData.append(0xCD);
                   uint16_t value = 4;
                   QByteArray valueBytes(reinterpret_cast<const char*>(&value), sizeof(value));
                   hexData.append(valueBytes);

                   serialPort->write(hexData);
                   serialPort->write(bcdData);

                   QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
                   qDebug()<<"写入数据："<<bcdData;
                   qDebug()<<"写入数据："<<bcdData.size();

            }else{

                QMessageBox::information(this, "写寄存器", "写允许未打开：WRTC1,WRTC2,WRTC3未置1!");

            }
       }else if (text=="SD2078") {
            sd2078Modelselt();
            QString binaryData;
            for (int row = 1; row <= 13; row +=2)
            {
                for (int col = 3; col < 11; col++)
                {
                    QString binaryValue = sd2078model->data(sd2078model->index(row, col)).toString();
                    binaryData += binaryValue;
                }
            }

            // 判断特定单元格的值是否为1
            if (sd2078model->data(sd2078model->index(31, 3)).toString() == "1" &&
                sd2078model->data(sd2078model->index(31, 8)).toString() == "1" &&
                sd2078model->data(sd2078model->index(33, 3)).toString() == "1"){
                // 将二进制数据转换为BCD码并发送给单片机
                   QByteArray bcdData;
                   for (int i = 0; i < binaryData.length(); i += 8)
                   {
                       QString binary = binaryData.mid(i, 8);
                       bool ok;
                       int decimal = binary.toInt(&ok, 2);
                       if (ok)
                       {
                           QString bcd = QString::number(decimal, 16);
                           bcdData.append(bcd.toInt(nullptr, 16));
                       }
                   }

                   QByteArray hexData;
                   hexData.append(0xBC);
                   hexData.append(0xCD);
                   uint16_t value = 4;
                   QByteArray valueBytes(reinterpret_cast<const char*>(&value), sizeof(value));
                   hexData.append(valueBytes);

                   serialPort->write(hexData);
                   serialPort->write(bcdData);

                   QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
                   qDebug()<<"写入数据："<<bcdData;
                   qDebug()<<"写入数据："<<bcdData.size();

            }else{

                QMessageBox::information(this, "写寄存器", "写允许未打开：WRTC1,WRTC2,WRTC3未置1!");

            }
       }else if (text=="SD8840") {
            sd8840Modelselt();
            QString binaryData;
                    for (int row = 1; row <= 13; row += 2)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd8840model->data(sd8840model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x91);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
       }else if (text=="SD2200"||text=="SD2201"||text=="SD2202"||text=="SD2203"||text=="SD2204")
        {
            sd2200Modelselt();
            QString binaryData;
            for (int row = 5; row <= 17; row += 2)
            {
                for (int col = 3; col < 11; col++)
                {
                    QString binaryValue = sd2200model->data(sd2200model->index(row, col)).toString();
                    binaryData += binaryValue;
                }
            }

            // 将二进制数据转换为BCD码并发送给单片机
            QByteArray bcdData;
            for (int i = 0; i < binaryData.length(); i += 8)
            {
                QString binary = binaryData.mid(i, 8);

                // 反转二进制字符串
                QString reversedBinary = binary;
                std::reverse(reversedBinary.begin(), reversedBinary.end());

                bool ok;
                int decimal = reversedBinary.toInt(&ok, 2);
                if (ok)
                {
                    QString bcd = QString::number(decimal, 16);
                    bcdData.append(bcd.toInt(nullptr, 16));
                }
            }
           QByteArray hexData;
           hexData.append(0xBC);
           hexData.append(0xCD);
           uint16_t value = 11;
           QByteArray valueBytes(reinterpret_cast<const char*>(&value), sizeof(value));
           hexData.append(valueBytes);

           serialPort->write(hexData);
           serialPort->write(bcdData);

           QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
           qDebug()<<"写入数据："<<bcdData;
           qDebug()<<"写入数据："<<bcdData.size();


        }

    }else{

        QMessageBox::information(this,"错误","串口打开失败，串口未连接或者被占用!");

    }


}

/**
功能：写3-3按键--写芯片寄存器，具体寄存器根据具体芯片型号来确定
修改：sjm
参数：-
返回值：-
**/
void RtcMessage::on_W07H_clicked()
{
    QString text = ui->rtctype->toPlainText();

    if(serialPort->isOpen())
    {
        if(text=="SD3068"||text=="SD3010"||text=="SD3025"||text=="SD3031"||text=="SD3077"||text=="SD3078"||text=="SD3178"||text=="SD3900"||text=="SD2503"||text=="SD2505"||text=="SD2506"||text=="SD2507"||text=="SD2508"||text=="SD2520"||text=="SD2522"||text=="SD2523")
        {
            sd3068Modelselt();
            QString binaryData;
                    for (int row = 1; row <= 63; row +=2)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd3068model->data(sd3068model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }
            // 判断特定单元格的值是否为1
            if (sd3068model->data(sd3068model->index(31, 3)).toString() == "1" &&
                sd3068model->data(sd3068model->index(31, 8)).toString() == "1" &&
                sd3068model->data(sd3068model->index(33, 3)).toString() == "1"){
                // 将二进制数据转换为BCD码并发送给单片机
                   QByteArray bcdData;
                   for (int i = 0; i < binaryData.length(); i += 8)
                   {
                       QString binary = binaryData.mid(i, 8);
                       bool ok;
                       int decimal = binary.toInt(&ok, 2);
                       if (ok)
                       {
                           QString bcd = QString::number(decimal, 16);
                           bcdData.append(bcd.toInt(nullptr, 16));
                       }
                   }

                   QByteArray hexData;
                   hexData.append(0xBC);
                   hexData.append(0xCD);
                   hexData.append(0xA4);
                   serialPort->write(hexData);
                   serialPort->write(bcdData);

                   QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
                   qDebug()<<"写入数据："<<bcdData;
                   qDebug()<<"写入数据："<<bcdData.size();

            }else{

                QMessageBox::information(this, "写寄存器", "写允许未打开：WRTC1,WRTC2,WRTC3未置1!");

            }


        }else if (text=="SD8563"||text=="SD8564"||text=="SD8565"||text=="SD8568") {
            sd8563Modelselt();
            QString binaryData;
                    for (int row = 1; row <= 31; row +=2)//从第7行索引开始，行从0开始
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd8563model->data(sd8563model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x13);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();

        }else if (text=="SD8810") {
            sd8810Modelselt();
            QString binaryData;
                    for (int row = 1; row <= 29; row += 2)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd8810model->data(sd8810model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x19);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();

        }else if (text=="SD8939") {
            sd8939Modelselt();
            QString binaryData;
                    for (int row = 1; row <= 39; row += 2)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd8939model->data(sd8939model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x31);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
        }else if (text=="SD8800") {
            sd8800Modelselt();
            QString binaryData;
                    for (int row = 1; row <= 31; row += 2)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd8800model->data(sd8800model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }
                       // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x41);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
        }else if (text=="SD8931") {
            sd8931Modelselt();
            QString binaryData;
                    for (int row = 1; row <= 39; row += 2)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd8931model->data(sd8931model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x53);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
        }else if (text=="SD8825") {
            sd8825Modelselt();
            QString binaryData;
                    for (int row = 1; row <= 39; row += 2)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd8825model->data(sd8825model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x63);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
        }else if (text=="SD8200") {
            sd8200Modelselt();
            QString binaryData;
                    for (int row = 1; row <= 33; row += 2)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd8200model->data(sd8200model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x73);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
        }else if (text=="SD8804") {
            sd8804Modelselt();
            QString binaryData;
                    for (int row = 1; row <= 31; row += 2)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd8804model->data(sd8804model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x3F);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
        }else if (text=="SD8902") {
            sd8902Modelselt();
            QString binaryData;
                    for (int row = 1; row <= 17; row += 2)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd8902model->data(sd8902model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x4F);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
        }else if (text=="SD2010") {
            sd2010Modelselt();
            QString binaryData;
            for (int row = 1; row <= 39; row +=2)
            {
                for (int col = 3; col < 11; col++)
                {
                    QString binaryValue = sd2010model->data(sd2010model->index(row, col)).toString();
                    binaryData += binaryValue;
                }
            }

            // 判断特定单元格的值是否为1
            if (sd2010model->data(sd2010model->index(31, 3)).toString() == "1" &&
                sd2010model->data(sd2010model->index(31, 8)).toString() == "1" &&
                sd2010model->data(sd2010model->index(33, 3)).toString() == "1"){
                // 将二进制数据转换为BCD码并发送给单片机
                   QByteArray bcdData;
                   for (int i = 0; i < binaryData.length(); i += 8)
                   {
                       QString binary = binaryData.mid(i, 8);
                       bool ok;
                       int decimal = binary.toInt(&ok, 2);
                       if (ok)
                       {
                           QString bcd = QString::number(decimal, 16);
                           bcdData.append(bcd.toInt(nullptr, 16));
                       }
                   }

                   QByteArray hexData;
                   hexData.append(0xBC);
                   hexData.append(0xCD);
                   uint16_t value = 6;
                   QByteArray valueBytes(reinterpret_cast<const char*>(&value), sizeof(value));
                   hexData.append(valueBytes);

                   serialPort->write(hexData);
                   serialPort->write(bcdData);

                   QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
                   qDebug()<<"写入数据："<<bcdData;
                   qDebug()<<"写入数据："<<bcdData.size();

            }else{

                QMessageBox::information(this, "写寄存器", "写允许未打开：WRTC1,WRTC2,WRTC3未置1!");

            }

        }else if (text=="SD2058") {
            sd2058Modelselt();
            QString binaryData;
            for (int row = 1; row <= 39; row +=2)
            {
                for (int col = 3; col < 11; col++)
                {
                    QString binaryValue = sd2058model->data(sd2058model->index(row, col)).toString();
                    binaryData += binaryValue;
                }
            }

            // 判断特定单元格的值是否为1
            if (sd2058model->data(sd2058model->index(31, 3)).toString() == "1" &&
                sd2058model->data(sd2058model->index(31, 8)).toString() == "1" &&
                sd2058model->data(sd2058model->index(33, 3)).toString() == "1"){
                // 将二进制数据转换为BCD码并发送给单片机
                   QByteArray bcdData;
                   for (int i = 0; i < binaryData.length(); i += 8)
                   {
                       QString binary = binaryData.mid(i, 8);
                       bool ok;
                       int decimal = binary.toInt(&ok, 2);
                       if (ok)
                       {
                           QString bcd = QString::number(decimal, 16);
                           bcdData.append(bcd.toInt(nullptr, 16));
                       }
                   }

                   QByteArray hexData;
                   hexData.append(0xBC);
                   hexData.append(0xCD);
                   hexData.append(0x81);

                   serialPort->write(hexData);
                   serialPort->write(bcdData);

                   QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
                   qDebug()<<"写入数据："<<bcdData;
                   qDebug()<<"写入数据："<<bcdData.size();

            }else{

                QMessageBox::information(this, "写寄存器", "写允许未打开：WRTC1,WRTC2,WRTC3未置1!");

            }
        }else if (text=="SD2059") {
            sd2059Modelselt();
            QString binaryData;
            for (int row = 1; row <= 39; row +=2)
            {
                for (int col = 3; col < 11; col++)
                {
                    QString binaryValue = sd2059model->data(sd2059model->index(row, col)).toString();
                    binaryData += binaryValue;
                }
            }

            // 判断特定单元格的值是否为1
            if (sd2059model->data(sd2059model->index(31, 3)).toString() == "1" &&
                sd2059model->data(sd2059model->index(31, 8)).toString() == "1" &&
                sd2059model->data(sd2059model->index(33, 3)).toString() == "1"){
                // 将二进制数据转换为BCD码并发送给单片机
                   QByteArray bcdData;
                   for (int i = 0; i < binaryData.length(); i += 8)
                   {
                       QString binary = binaryData.mid(i, 8);
                       bool ok;
                       int decimal = binary.toInt(&ok, 2);
                       if (ok)
                       {
                           QString bcd = QString::number(decimal, 16);
                           bcdData.append(bcd.toInt(nullptr, 16));
                       }
                   }

                   QByteArray hexData;
                   hexData.append(0xBC);
                   hexData.append(0xCD);
                   hexData.append(0x81);

                   serialPort->write(hexData);
                   serialPort->write(bcdData);

                   QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
                   qDebug()<<"写入数据："<<bcdData;
                   qDebug()<<"写入数据："<<bcdData.size();

            }else{

                QMessageBox::information(this, "写寄存器", "写允许未打开：WRTC1,WRTC2,WRTC3未置1!");

            }
        }else if (text=="SD2068"||text=="SD2400"||text=="SD2401"||text=="SD2402"||text=="SD2403"||text=="SD2404"||text=="SD2405") {
            sd2068Modelselt();
            QString binaryData;
            for (int row = 1; row <= 39; row +=2)
            {
                for (int col = 3; col < 11; col++)
                {
                    QString binaryValue = sd2068model->data(sd2068model->index(row, col)).toString();
                    binaryData += binaryValue;
                }
            }

            // 判断特定单元格的值是否为1
            if (sd2068model->data(sd2068model->index(31, 3)).toString() == "1" &&
                sd2068model->data(sd2068model->index(31, 8)).toString() == "1" &&
                sd2068model->data(sd2068model->index(33, 3)).toString() == "1"){
                // 将二进制数据转换为BCD码并发送给单片机
                   QByteArray bcdData;
                   for (int i = 0; i < binaryData.length(); i += 8)
                   {
                       QString binary = binaryData.mid(i, 8);
                       bool ok;
                       int decimal = binary.toInt(&ok, 2);
                       if (ok)
                       {
                           QString bcd = QString::number(decimal, 16);
                           bcdData.append(bcd.toInt(nullptr, 16));
                       }
                   }

                   QByteArray hexData;
                   hexData.append(0xBC);
                   hexData.append(0xCD);
                   hexData.append(0x81);

                   serialPort->write(hexData);
                   serialPort->write(bcdData);

                   QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
                   qDebug()<<"写入数据："<<bcdData;
                   qDebug()<<"写入数据："<<bcdData.size();

            }else{

                QMessageBox::information(this, "写寄存器", "写允许未打开：WRTC1,WRTC2,WRTC3未置1!");

            }
        }else if (text=="SD2069") {
            sd2069Modelselt();
            QString binaryData;
            for (int row = 1; row <= 39; row +=2)
            {
                for (int col = 3; col < 11; col++)
                {
                    QString binaryValue = sd2069model->data(sd2069model->index(row, col)).toString();
                    binaryData += binaryValue;
                }
            }

            // 判断特定单元格的值是否为1
            if (sd2069model->data(sd2069model->index(31, 3)).toString() == "1" &&
                sd2069model->data(sd2069model->index(31, 8)).toString() == "1" &&
                sd2069model->data(sd2069model->index(33, 3)).toString() == "1"){
                // 将二进制数据转换为BCD码并发送给单片机
                   QByteArray bcdData;
                   for (int i = 0; i < binaryData.length(); i += 8)
                   {
                       QString binary = binaryData.mid(i, 8);
                       bool ok;
                       int decimal = binary.toInt(&ok, 2);
                       if (ok)
                       {
                           QString bcd = QString::number(decimal, 16);
                           bcdData.append(bcd.toInt(nullptr, 16));
                       }
                   }

                   QByteArray hexData;
                   hexData.append(0xBC);
                   hexData.append(0xCD);
                   hexData.append(0x81);

                   serialPort->write(hexData);
                   serialPort->write(bcdData);

                   QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
                   qDebug()<<"写入数据："<<bcdData;
                   qDebug()<<"写入数据："<<bcdData.size();

            }else{

                QMessageBox::information(this, "写寄存器", "写允许未打开：WRTC1,WRTC2,WRTC3未置1!");

            }
        }else if (text=="SD2077") {
            sd2077Modelselt();
            QString binaryData;
            for (int row = 1; row <= 39; row +=2)
            {
                for (int col = 3; col < 11; col++)
                {
                    QString binaryValue = sd2077model->data(sd2077model->index(row, col)).toString();
                    binaryData += binaryValue;
                }
            }

            // 判断特定单元格的值是否为1
            if (sd2077model->data(sd2077model->index(31, 3)).toString() == "1" &&
                sd2077model->data(sd2077model->index(31, 8)).toString() == "1" &&
                sd2077model->data(sd2077model->index(33, 3)).toString() == "1"){
                // 将二进制数据转换为BCD码并发送给单片机
                   QByteArray bcdData;
                   for (int i = 0; i < binaryData.length(); i += 8)
                   {
                       QString binary = binaryData.mid(i, 8);
                       bool ok;
                       int decimal = binary.toInt(&ok, 2);
                       if (ok)
                       {
                           QString bcd = QString::number(decimal, 16);
                           bcdData.append(bcd.toInt(nullptr, 16));
                       }
                   }

                   QByteArray hexData;
                   hexData.append(0xBC);
                   hexData.append(0xCD);
                   hexData.append(0x81);

                   serialPort->write(hexData);
                   serialPort->write(bcdData);

                   QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
                   qDebug()<<"写入数据："<<bcdData;
                   qDebug()<<"写入数据："<<bcdData.size();

            }else{

                QMessageBox::information(this, "写寄存器", "写允许未打开：WRTC1,WRTC2,WRTC3未置1!");

            }
        }else if (text=="SD2078") {
            sd2078Modelselt();
            QString binaryData;
            for (int row = 1; row <= 39; row +=2)
            {
                for (int col = 3; col < 11; col++)
                {
                    QString binaryValue = sd2078model->data(sd2078model->index(row, col)).toString();
                    binaryData += binaryValue;
                }
            }

            // 判断特定单元格的值是否为1
            if (sd2078model->data(sd2078model->index(31, 3)).toString() == "1" &&
                sd2078model->data(sd2078model->index(31, 8)).toString() == "1" &&
                sd2078model->data(sd2078model->index(33, 3)).toString() == "1"){
                // 将二进制数据转换为BCD码并发送给单片机
                   QByteArray bcdData;
                   for (int i = 0; i < binaryData.length(); i += 8)
                   {
                       QString binary = binaryData.mid(i, 8);
                       bool ok;
                       int decimal = binary.toInt(&ok, 2);
                       if (ok)
                       {
                           QString bcd = QString::number(decimal, 16);
                           bcdData.append(bcd.toInt(nullptr, 16));
                       }
                   }

                   QByteArray hexData;
                   hexData.append(0xBC);
                   hexData.append(0xCD);
                   hexData.append(0x81);

                   serialPort->write(hexData);
                   serialPort->write(bcdData);

                   QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
                   qDebug()<<"写入数据："<<bcdData;
                   qDebug()<<"写入数据："<<bcdData.size();

            }else{

                QMessageBox::information(this, "写寄存器", "写允许未打开：WRTC1,WRTC2,WRTC3未置1!");

            }
        }else if (text=="SD8840") {
            sd8840Modelselt();
            QString binaryData;
                    for (int row = 1; row <= 31; row += 2)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd8840model->data(sd8840model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x93);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
        }else if (text=="SD2200"||text=="SD2201"||text=="SD2202"||text=="SD2203"||text=="SD2204"){
            sd2200Modelselt();
            QString binaryData;

            // 遍历行，注意这里 row 从 1 开始，并且按 2 遍历
            for (int row = 1; row <= 37; row += 2)
            {
                for (int col = 3; col < 11; col++)
                {
                    QString binaryValue = sd2200model->data(sd2200model->index(row, col)).toString();

                    // 检查当前行是否在5到17之间
                    if (row >= 5 && row <= 17) {
                        // 反转二进制字符串
                        std::reverse(binaryValue.begin(), binaryValue.end());
                    }

                    // 拼接二进制值
                    binaryData += binaryValue;
                }
            }

            // 将二进制数据转换为BCD码并发送给单片机
            QByteArray bcdData;
            for (int i = 0; i < binaryData.length(); i += 8)
            {
                QString binary = binaryData.mid(i, 8);
                bool ok;
                int decimal = binary.toInt(&ok, 2);
                if (ok)
                {
                    QString bcd = QString::number(decimal, 16);
                    bcdData.append(bcd.toInt(nullptr, 16));
                }
            }

           QByteArray hexData;
           hexData.append(0xBC);
           hexData.append(0xCD);
           uint16_t value = 12;
           QByteArray valueBytes(reinterpret_cast<const char*>(&value), sizeof(value));
           hexData.append(valueBytes);

           serialPort->write(hexData);
           serialPort->write(bcdData);

           QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
           qDebug()<<"写入数据："<<bcdData;
           qDebug()<<"写入数据："<<bcdData.size();

        }

    }else{

        QMessageBox::information(this,"错误","串口打开失败，串口未连接或者被占用!");

    }


}

/**
功能：写4-4按键--写芯片寄存器，具体寄存器根据具体芯片型号来确定
修改：sjm
参数：-
返回值：-
**/
void RtcMessage::on_W2CH_clicked()
{
     QString text = ui->rtctype->toPlainText();

    if(serialPort->isOpen())
    {
        if(text=="SD3068"||text=="SD3010"||text=="SD3025"||text=="SD3031"||text=="SD3077"||text=="SD3078"||text=="SD3178"||text=="SD3900"||text=="SD2503"||text=="SD2505"||text=="SD2506"||text=="SD2507"||text=="SD2508"||text=="SD2520"||text=="SD2522"||text=="SD2523")
        {
            sd3068Modelselt();
            QString binaryData;
                    for (int row = 76; row <= 95; row ++)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd3068model->data(sd3068model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x84);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
        }else if (text=="SD8810") {
            sd8810Modelselt();
            QString binaryData;
                    for (int row = 31; row <= 73; row += 2)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd8810model->data(sd8810model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x21);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
        }else if (text=="SD8939") {
            sd8939Modelselt();
            QString binaryData;
                    for (int row = 41; row <= 195; row += 2)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd8939model->data(sd8939model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x33);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
        }else if (text=="SD8800") {
            sd8800Modelselt();
            QString binaryData;
                    for (int row = 33; row <= 53; row += 2)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd8800model->data(sd8800model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x43);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
        }else if (text=="SD8931") {
            sd8931Modelselt();
            QString binaryData;
                    for (int row = 41; row <= 195; row += 2)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd8931model->data(sd8931model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x55);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
        }else if (text=="SD8825") {
            sd8825Modelselt();
            QString binaryData;
                    for (int row = 41; row <= 193; row += 2)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd8825model->data(sd8825model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x65);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
        }else if (text=="SD8200") {
            sd8200Modelselt();
            QString binaryData;
                    for (int row = 35; row <= 189; row += 2)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd8200model->data(sd8200model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x75);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
        }else if (text=="SD8804") {
            sd8804Modelselt();
            QString binaryData;
                    for (int row = 33; row <= 79; row +=2)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd8804model->data(sd8804model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x85);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
        }else if (text=="SD8902") {
            sd8902Modelselt();
            QString binaryData;
                    for (int row = 39; row <= 41; row += 2)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd8902model->data(sd8902model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x5A);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
        }else if (text=="SD2010") {
            sd2010Modelselt();
            QString binaryData;
            for (int row = 41; row <= 63; row +=2)
            {
                for (int col = 3; col < 11; col++)
                {
                    QString binaryValue = sd2010model->data(sd2010model->index(row, col)).toString();
                    binaryData += binaryValue;
                }
            }
                // 将二进制数据转换为BCD码并发送给单片机
                   QByteArray bcdData;
                   for (int i = 0; i < binaryData.length(); i += 8)
                   {
                       QString binary = binaryData.mid(i, 8);
                       bool ok;
                       int decimal = binary.toInt(&ok, 2);
                       if (ok)
                       {
                           QString bcd = QString::number(decimal, 16);
                           bcdData.append(bcd.toInt(nullptr, 16));
                       }
                   }

                   QByteArray hexData;
                   hexData.append(0xBC);
                   hexData.append(0xCD);
                   uint16_t value = 7;
                   QByteArray valueBytes(reinterpret_cast<const char*>(&value), sizeof(value));
                   hexData.append(valueBytes);

                   serialPort->write(hexData);
                   serialPort->write(bcdData);

                   QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
                   qDebug()<<"写入数据："<<bcdData;
                   qDebug()<<"写入数据："<<bcdData.size();
        }else if (text=="SD2058") {
            sd2058Modelselt();
            QString binaryData;
            for (int row = 41; row <= 127; row +=2)
            {
                for (int col = 3; col < 11; col++)
                {
                    QString binaryValue = sd2058model->data(sd2058model->index(row, col)).toString();
                    binaryData += binaryValue;
                }
            }
                // 将二进制数据转换为BCD码并发送给单片机
                   QByteArray bcdData;
                   for (int i = 0; i < binaryData.length(); i += 8)
                   {
                       QString binary = binaryData.mid(i, 8);
                       bool ok;
                       int decimal = binary.toInt(&ok, 2);
                       if (ok)
                       {
                           QString bcd = QString::number(decimal, 16);
                           bcdData.append(bcd.toInt(nullptr, 16));
                       }
                   }

                   QByteArray hexData;
                   hexData.append(0xBC);
                   hexData.append(0xCD);
                   hexData.append(0x82);

                   serialPort->write(hexData);
                   serialPort->write(bcdData);

                   QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
                   qDebug()<<"写入数据："<<bcdData;
                   qDebug()<<"写入数据："<<bcdData.size();
        }else if (text=="SD2059") {
            sd2059Modelselt();
            QString binaryData;
            for (int row = 41; row <= 127; row +=2)
            {
                for (int col = 3; col < 11; col++)
                {
                    QString binaryValue = sd2059model->data(sd2059model->index(row, col)).toString();
                    binaryData += binaryValue;
                }
            }
                // 将二进制数据转换为BCD码并发送给单片机
                   QByteArray bcdData;
                   for (int i = 0; i < binaryData.length(); i += 8)
                   {
                       QString binary = binaryData.mid(i, 8);
                       bool ok;
                       int decimal = binary.toInt(&ok, 2);
                       if (ok)
                       {
                           QString bcd = QString::number(decimal, 16);
                           bcdData.append(bcd.toInt(nullptr, 16));
                       }
                   }

                   QByteArray hexData;
                   hexData.append(0xBC);
                   hexData.append(0xCD);
                   hexData.append(0x82);

                   serialPort->write(hexData);
                   serialPort->write(bcdData);

                   QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
                   qDebug()<<"写入数据："<<bcdData;
                   qDebug()<<"写入数据："<<bcdData.size();
        }else if (text=="SD2068"||text=="SD2400"||text=="SD2401"||text=="SD2402"||text=="SD2403"||text=="SD2404"||text=="SD2405") {
            sd2068Modelselt();
            QString binaryData;
            for (int row = 41; row <= 63; row +=2)
            {
                for (int col = 3; col < 11; col++)
                {
                    QString binaryValue = sd2068model->data(sd2068model->index(row, col)).toString();
                    binaryData += binaryValue;
                }
            }
                // 将二进制数据转换为BCD码并发送给单片机
                   QByteArray bcdData;
                   for (int i = 0; i < binaryData.length(); i += 8)
                   {
                       QString binary = binaryData.mid(i, 8);
                       bool ok;
                       int decimal = binary.toInt(&ok, 2);
                       if (ok)
                       {
                           QString bcd = QString::number(decimal, 16);
                           bcdData.append(bcd.toInt(nullptr, 16));
                       }
                   }

                   QByteArray hexData;
                   hexData.append(0xBC);
                   hexData.append(0xCD);
                   hexData.append(0x82);

                   serialPort->write(hexData);
                   serialPort->write(bcdData);

                   QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
                   qDebug()<<"写入数据："<<bcdData;
                   qDebug()<<"写入数据："<<bcdData.size();
        }else if (text=="SD2069") {
            sd2069Modelselt();
            QString binaryData;
            for (int row = 41; row <= 63; row +=2)
            {
                for (int col = 3; col < 11; col++)
                {
                    QString binaryValue = sd2069model->data(sd2069model->index(row, col)).toString();
                    binaryData += binaryValue;
                }
            }
                // 将二进制数据转换为BCD码并发送给单片机
                   QByteArray bcdData;
                   for (int i = 0; i < binaryData.length(); i += 8)
                   {
                       QString binary = binaryData.mid(i, 8);
                       bool ok;
                       int decimal = binary.toInt(&ok, 2);
                       if (ok)
                       {
                           QString bcd = QString::number(decimal, 16);
                           bcdData.append(bcd.toInt(nullptr, 16));
                       }
                   }

                   QByteArray hexData;
                   hexData.append(0xBC);
                   hexData.append(0xCD);
                   hexData.append(0x82);

                   serialPort->write(hexData);
                   serialPort->write(bcdData);

                   QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
                   qDebug()<<"写入数据："<<bcdData;
                   qDebug()<<"写入数据："<<bcdData.size();
        }else if (text=="SD2077") {
            sd2077Modelselt();
            QString binaryData;
            for (int row = 41; row <= 63; row +=2)
            {
                for (int col = 3; col < 11; col++)
                {
                    QString binaryValue = sd2077model->data(sd2077model->index(row, col)).toString();
                    binaryData += binaryValue;
                }
            }
                // 将二进制数据转换为BCD码并发送给单片机
                   QByteArray bcdData;
                   for (int i = 0; i < binaryData.length(); i += 8)
                   {
                       QString binary = binaryData.mid(i, 8);
                       bool ok;
                       int decimal = binary.toInt(&ok, 2);
                       if (ok)
                       {
                           QString bcd = QString::number(decimal, 16);
                           bcdData.append(bcd.toInt(nullptr, 16));
                       }
                   }

                   QByteArray hexData;
                   hexData.append(0xBC);
                   hexData.append(0xCD);
                   hexData.append(0x82);

                   serialPort->write(hexData);
                   serialPort->write(bcdData);

                   QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
                   qDebug()<<"写入数据："<<bcdData;
                   qDebug()<<"写入数据："<<bcdData.size();
        }else if (text=="SD2078") {
            sd2078Modelselt();
            QString binaryData;
            for (int row = 41; row <= 63; row +=2)
            {
                for (int col = 3; col < 11; col++)
                {
                    QString binaryValue = sd2078model->data(sd2078model->index(row, col)).toString();
                    binaryData += binaryValue;
                }
            }
                // 将二进制数据转换为BCD码并发送给单片机
                   QByteArray bcdData;
                   for (int i = 0; i < binaryData.length(); i += 8)
                   {
                       QString binary = binaryData.mid(i, 8);
                       bool ok;
                       int decimal = binary.toInt(&ok, 2);
                       if (ok)
                       {
                           QString bcd = QString::number(decimal, 16);
                           bcdData.append(bcd.toInt(nullptr, 16));
                       }
                   }

                   QByteArray hexData;
                   hexData.append(0xBC);
                   hexData.append(0xCD);
                   hexData.append(0x82);

                   serialPort->write(hexData);
                   serialPort->write(bcdData);

                   QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
                   qDebug()<<"写入数据："<<bcdData;
                   qDebug()<<"写入数据："<<bcdData.size();
        }else if (text=="SD8840") {
            sd8840Modelselt();
            QString binaryData;
                    for (int row = 33; row <= 49; row += 2)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd8840model->data(sd8840model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x95);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
        }

    }else{

        QMessageBox::information(this,"错误","串口打开失败，串口未连接或者被占用!");

    }

}

/**
功能：写5-5按键--写芯片寄存器，具体寄存器根据具体芯片型号来确定
修改：sjm
参数：-
返回值：-
**/
void RtcMessage::on_W40H_clicked()
{
    QString text = ui->rtctype->toPlainText();

    if(serialPort->isOpen())
    {
        if(text=="SD3068"||text=="SD3010"||text=="SD3025"||text=="SD3031"||text=="SD3077"||text=="SD3078"||text=="SD3178"||text=="SD3900"||text=="SD2503"||text=="SD2505"||text=="SD2506"||text=="SD2507"||text=="SD2508"||text=="SD2520"||text=="SD2522"||text=="SD2523")
        {
            sd3068Modelselt();
            QString binaryData;
                    for (int row = 94; row <= 125; row ++)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd3068model->data(sd3068model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0xAD);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
        }else if (text=="SD8810") {
            sd8810Modelselt();
            QString binaryData;
                    for (int row = 75; row <= 181; row += 2)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd8810model->data(sd8810model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x23);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
        }else if (text=="SD8939") {
            sd8939Modelselt();
            QString binaryData;
                    for (int row = 197; row <= 203; row += 2)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd8939model->data(sd8939model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x35);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
        }else if (text=="SD8800") {
            sd8800Modelselt();
            QString binaryData;
                    for (int row = 54; row <= 122; row ++)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd8800model->data(sd8800model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x45);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
        }else if (text=="SD8931") {
            sd8931Modelselt();
            QString binaryData;
                    for (int row = 197; row <= 203; row += 2)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd8931model->data(sd8931model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x57);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
        }else if (text=="SD8825") {
            sd8825Modelselt();
            QString binaryData;
                    for (int row = 195; row <= 201; row += 2)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd8825model->data(sd8825model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x67);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
        }else if (text=="SD8200") {
            sd8200Modelselt();
            QString binaryData;
                    for (int row = 191; row <= 197; row += 2)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd8200model->data(sd8200model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x77);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
        }else if (text=="SD8804") {
            sd8804Modelselt();
            QString binaryData;
                    for (int row = 80; row <= 148; row ++)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd8804model->data(sd8804model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x87);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
        }else if (text=="SD8902") {
            sd8902Modelselt();
            QString binaryData;
                    for (int row = 47; row <= 49; row += 2)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd8902model->data(sd8902model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x5B);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
        }else if (text=="SD8840") {
            sd8840Modelselt();
            QString binaryData;
                    for (int row = 51; row <= 87; row += 2)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd8840model->data(sd8840model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x97);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
        }

    }else{

        QMessageBox::information(this,"错误","串口打开失败，串口未连接或者被占用!");

    }


}

/**
功能：写6-6按键--写芯片寄存器，具体寄存器根据具体芯片型号来确定
修改：sjm
参数：-
返回值：-
**/
void RtcMessage::on_W60H_clicked()
{
    QString text = ui->rtctype->toPlainText();

    if(serialPort->isOpen())
    {
        if(text=="SD3068"||text=="SD3010"||text=="SD3025"||text=="SD3031"||text=="SD3077"||text=="SD3078"||text=="SD3178"||text=="SD3900"||text=="SD2503"||text=="SD2505"||text=="SD2506"||text=="SD2507"||text=="SD2508"||text=="SD2520"||text=="SD2522"||text=="SD2523")
         {
            sd3068Modelselt();
             QString binaryData;
                     for (int row = 122; row <= 153; row ++)
                     {
                         for (int col = 3; col < 11; col++)
                         {
                             QString binaryValue = sd3068model->data(sd3068model->index(row, col)).toString();
                             binaryData += binaryValue;
                         }
                     }

                     // 将二进制数据转换为BCD码并发送给单片机
                        QByteArray bcdData;
                        for (int i = 0; i < binaryData.length(); i += 8)
                        {
                            QString binary = binaryData.mid(i, 8);
                            bool ok;
                            int decimal = binary.toInt(&ok, 2);
                            if (ok)
                            {
                                QString bcd = QString::number(decimal, 16);
                                bcdData.append(bcd.toInt(nullptr, 16));
                            }
                        }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0xAF);
            serialPort->write(hexData);
            serialPort->write(bcdData);

             QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
             qDebug()<<"写入数据："<<bcdData;
             qDebug()<<"写入数据："<<bcdData.size();
         }else if (text=="SD8810") {
             sd8810Modelselt();
             QString binaryData;
                     for (int row = 199; row <= 205; row += 2)
                     {
                         for (int col = 3; col < 11; col++)
                         {
                             QString binaryValue = sd8810model->data(sd8810model->index(row, col)).toString();
                             binaryData += binaryValue;
                         }
                     }

                     // 将二进制数据转换为BCD码并发送给单片机
                        QByteArray bcdData;
                        for (int i = 0; i < binaryData.length(); i += 8)
                        {
                            QString binary = binaryData.mid(i, 8);
                            bool ok;
                            int decimal = binary.toInt(&ok, 2);
                            if (ok)
                            {
                                QString bcd = QString::number(decimal, 16);
                                bcdData.append(bcd.toInt(nullptr, 16));
                            }
                        }

             QByteArray hexData;
             hexData.append(0xBC);
             hexData.append(0xCD);
             hexData.append(0x25);
             serialPort->write(hexData);
             serialPort->write(bcdData);

             QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
             qDebug()<<"写入数据："<<bcdData;
             qDebug()<<"写入数据："<<bcdData.size();
         }else if (text=="SD8800") {
             sd8800Modelselt();
             QString binaryData;
                     for (int row = 164; row <= 188; row += 2)
                     {
                         for (int col = 3; col < 11; col++)
                         {
                             QString binaryValue = sd8800model->data(sd8800model->index(row, col)).toString();
                             binaryData += binaryValue;
                         }
                     }

                     // 将二进制数据转换为BCD码并发送给单片机
                        QByteArray bcdData;
                        for (int i = 0; i < binaryData.length(); i += 8)
                        {
                            QString binary = binaryData.mid(i, 8);
                            bool ok;
                            int decimal = binary.toInt(&ok, 2);
                            if (ok)
                            {
                                QString bcd = QString::number(decimal, 16);
                                bcdData.append(bcd.toInt(nullptr, 16));
                            }
                        }

             QByteArray hexData;
             hexData.append(0xBC);
             hexData.append(0xCD);
             hexData.append(0x47);
             serialPort->write(hexData);
             serialPort->write(bcdData);

             QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
             qDebug()<<"写入数据："<<bcdData;
             qDebug()<<"写入数据："<<bcdData.size();
         }else if (text=="SD8804") {
            sd8804Modelselt();
            QString binaryData;
                    for (int row = 190; row <= 196; row += 2)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd8804model->data(sd8804model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x47);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
         }else if (text=="SD8902") {
            sd8902Modelselt();
            QString binaryData;
                    for (int row = 51; row <= 111; row += 2)
                    {
                        for (int col = 3; col < 11; col++)
                        {
                            QString binaryValue = sd8902model->data(sd8902model->index(row, col)).toString();
                            binaryData += binaryValue;
                        }
                    }

                    // 将二进制数据转换为BCD码并发送给单片机
                       QByteArray bcdData;
                       for (int i = 0; i < binaryData.length(); i += 8)
                       {
                           QString binary = binaryData.mid(i, 8);
                           bool ok;
                           int decimal = binary.toInt(&ok, 2);
                           if (ok)
                           {
                               QString bcd = QString::number(decimal, 16);
                               bcdData.append(bcd.toInt(nullptr, 16));
                           }
                       }

            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x5C);
            serialPort->write(hexData);
            serialPort->write(bcdData);

            QMessageBox::information(this, "写寄存器", "写取寄存器成功！");
            qDebug()<<"写入数据："<<bcdData;
            qDebug()<<"写入数据："<<bcdData.size();
        }

        }else{

        QMessageBox::information(this,"错误","串口打开失败，串口未连接或者被占用!");

    }
}

/*********************************************************读芯片温度，电池电压，id码槽函数实现***************************************************************/
/**
功能：读芯片的温度
修改：sjm
参数：-
返回值：-
**/
void RtcMessage::on_tempButton_clicked()
{
    QString text = ui->rtctype->toPlainText();
    //清空输入缓冲区
    serialPort->clear(QSerialPort::Input);

    QFont font;
    font.setPointSize(20); // 设置字体大小为12

    if(serialPort->isOpen())
    {
        if(text=="SD3068"||text=="SD3010"||text=="SD3025"||text=="SD3031"||text=="SD3077"||text=="SD3078"||text=="SD3178"||text=="SD3900"||text=="SD2503"||text=="SD2505"||text=="SD2506"||text=="SD2507"||text=="SD2508"||text=="SD2520"||text=="SD2522"||text=="SD2523")
        {
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0xF4);
            serialPort->write(hexData);

            QEventLoop loop;
            QObject::connect(serialPort, &QSerialPort::readyRead, &loop, &QEventLoop::quit);//串口等待芯片的温度数据发送
            loop.exec();

            QByteArray readtemp = serialPort->readAll();
            qDebug() << "收到数据：" << readtemp;
            qDebug() << readtemp.size();
            //处理芯片温度数据
            QString tempdecimal;
            if (!readtemp.isEmpty()) {
                if (readtemp.size() == 2) {
                    int decimalValue1 = readtemp[0];
                    // 转换为十进制
                    QString temp1 = QString::number(decimalValue1, 10);
                    // 显示十进制值
                    tempdecimal = QString("%1℃").arg(temp1);
                    ui->temp->setFont(font);
                    ui->temp->setText(tempdecimal);
                    ui->temp->setAlignment(Qt::AlignCenter);
                    QMessageBox::information(this, "温度读取", "温度读取成功！");
                }

            }

        qDebug() << "温度数据：" << tempdecimal;

        }else if (text=="SD8200"||text=="SD2010"||text=="SD2058"||text=="SD2059"||text=="SD2068"||text=="SD2069"||text=="SD2077"||text=="SD2078"||text=="SD2400"||text=="SD2401"||text=="SD2402"||text=="SD2403"||text=="SD2404"||text=="SD2405"||text=="SD8563"||\
                  text=="SD2200"||text=="SD2201"||text=="SD2202"||text=="SD2203"||text=="SD2204") {
                     QMessageBox::information(this, "温度读取", "当前所选型号没有此功能！");
        }else if (text=="SD8810") {
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x1A);
            serialPort->write(hexData);

            QEventLoop loop;
            QObject::connect(serialPort, &QSerialPort::readyRead, &loop, &QEventLoop::quit);
            loop.exec();

            QByteArray readtemp = serialPort->readAll();
            qDebug() << "收到数据：" << readtemp;
            qDebug() << readtemp.size();

            QString tempdecimal;
            if (!readtemp.isEmpty()) {
                if (readtemp.size() == 2) {
                    int decimalValue1 = readtemp[0];
                    // 转换为十进制
                    QString temp1 = QString::number(decimalValue1, 10);
                    // 显示十进制值
                    tempdecimal = QString("%1℃").arg(temp1);
                    ui->temp->setFont(font);
                    ui->temp->setText(tempdecimal);
                    ui->temp->setAlignment(Qt::AlignCenter);
                    QMessageBox::information(this, "温度读取", "温度读取成功！");
                }

            }

        qDebug() << "温度数据：" << tempdecimal;
        }else if (text=="SD8800") {
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x1C);
            serialPort->write(hexData);

            QEventLoop loop;
            QObject::connect(serialPort, &QSerialPort::readyRead, &loop, &QEventLoop::quit);
            loop.exec();

            QByteArray readtemp = serialPort->readAll();
            qDebug() << "收到数据：" << readtemp;
            qDebug() << readtemp.size();

            QString tempdecimal;
            if (!readtemp.isEmpty()) {
                if (readtemp.size() == 2) {
                    int decimalValue1 = readtemp[0];
                    // 转换为十进制
                    QString temp1 = QString::number(decimalValue1, 10);
                    // 显示十进制值
                    tempdecimal = QString("%1℃").arg(temp1);
                    ui->temp->setFont(font);
                    ui->temp->setText(tempdecimal);
                    ui->temp->setAlignment(Qt::AlignCenter);
                    QMessageBox::information(this, "温度读取", "温度读取成功！");
                }

            }

        qDebug() << "温度数据：" << tempdecimal;
        }else if (text=="SD8931") {
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x1E);
            serialPort->write(hexData);

            QEventLoop loop;
            QObject::connect(serialPort, &QSerialPort::readyRead, &loop, &QEventLoop::quit);
            loop.exec();

            QByteArray readtemp = serialPort->readAll();
            qDebug() << "收到数据：" << readtemp;
            qDebug() << readtemp.size();

            QString tempdecimal;
            if (!readtemp.isEmpty()) {
                if (readtemp.size() == 2) {
                    int decimalValue1 = readtemp[0];
                    // 转换为十进制
                    QString temp1 = QString::number(decimalValue1, 10);
                    // 显示十进制值
                    tempdecimal = QString("%1℃").arg(temp1);
                    ui->temp->setFont(font);
                    ui->temp->setText(tempdecimal);
                    ui->temp->setAlignment(Qt::AlignCenter);
                    QMessageBox::information(this, "温度读取", "温度读取成功！");
                }

            }

            qDebug() << "温度数据：" << tempdecimal;
        }else if (text=="SD8825") {
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x1F);
            serialPort->write(hexData);

            QEventLoop loop;
            QObject::connect(serialPort, &QSerialPort::readyRead, &loop, &QEventLoop::quit);
            loop.exec();

            QByteArray readtemp = serialPort->readAll();
            qDebug() << "收到数据：" << readtemp;
            qDebug() << readtemp.size();

            QString tempdecimal;
            if (!readtemp.isEmpty()) {
                if (readtemp.size() == 2) {
                    int decimalValue1 = readtemp[0];
                    // 转换为十进制
                    QString temp1 = QString::number(decimalValue1, 10);
                    // 显示十进制值
                    tempdecimal = QString("%1℃").arg(temp1);
                    ui->temp->setFont(font);
                    ui->temp->setText(tempdecimal);
                    ui->temp->setAlignment(Qt::AlignCenter);
                    QMessageBox::information(this, "温度读取", "温度读取成功！");
                }

            }

            qDebug() << "温度数据：" << tempdecimal;
        }else if (text=="SD8840") {
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x2C);
            serialPort->write(hexData);

            QEventLoop loop;
            QObject::connect(serialPort, &QSerialPort::readyRead, &loop, &QEventLoop::quit);
            loop.exec();

            QByteArray readtemp = serialPort->readAll();
            qDebug() << "收到数据：" << readtemp;
            qDebug() << readtemp.size();

            QString tempdecimal;
            if (!readtemp.isEmpty()) {
                if (readtemp.size() == 2) {
                    int decimalValue1 = readtemp[0];
                    // 转换为十进制
                    QString temp1 = QString::number(decimalValue1, 10);
                    // 显示十进制值
                    tempdecimal = QString("%1℃").arg(temp1);
                    ui->temp->setFont(font);
                    ui->temp->setText(tempdecimal);
                    ui->temp->setAlignment(Qt::AlignCenter);
                    QMessageBox::information(this, "温度读取", "温度读取成功！");
                }

            }

            qDebug() << "温度数据：" << tempdecimal;
        }else if (text=="SD8902") {
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x2D);
            serialPort->write(hexData);

            QEventLoop loop;
            QObject::connect(serialPort, &QSerialPort::readyRead, &loop, &QEventLoop::quit);
            loop.exec();

            QByteArray readtemp = serialPort->readAll();
            qDebug() << "收到数据：" << readtemp;
            qDebug() << readtemp.size();

            QString tempdecimal;
            if (!readtemp.isEmpty()) {
                if (readtemp.size() == 2) {
                    int decimalValue1 = readtemp[0];
                    // 转换为十进制
                    QString temp1 = QString::number(decimalValue1, 10);
                    // 显示十进制值
                    tempdecimal = QString("%1℃").arg(temp1);
                    ui->temp->setFont(font);
                    ui->temp->setText(tempdecimal);
                    ui->temp->setAlignment(Qt::AlignCenter);
                    QMessageBox::information(this, "温度读取", "温度读取成功！");
                }

            }

            qDebug() << "温度数据：" << tempdecimal;
        }

         }else{

            QMessageBox::information(this,"错误","串口打开失败，串口未连接或者被占用!");

        }
}

/**
功能：读芯片的电池电压
修改：sjm
参数：-
返回值：-
**/
void RtcMessage::on_batButton_clicked()
{
    QString text = ui->rtctype->toPlainText();
    //清空输入缓冲区
    serialPort->clear(QSerialPort::Input);

    QFont font;
    font.setPointSize(20); // 设置字体大小为12

    if(serialPort->isOpen())
    {
        if(text=="SD3068"||text=="SD3010"||text=="SD3025"||text=="SD3031"||text=="SD3077"||text=="SD3078"||text=="SD3178"||text=="SD3900"||text=="SD2503"||text=="SD2505"||text=="SD2506"||text=="SD2507"||text=="SD2508"||text=="SD2520"||text=="SD2522"||text=="SD2523")
        {
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0xF5);
            serialPort->write(hexData);

            QEventLoop loop;
            QObject::connect(serialPort, &QSerialPort::readyRead, &loop, &QEventLoop::quit);//串口等待芯片电池电压数据发送
            loop.exec();

            QByteArray readbat=serialPort->readAll();
            qDebug()<<"收到数据："<<readbat;
            qDebug() <<readbat.size();
            //处理电池电压数据
            QString batdecimal;
            if (!readbat.isEmpty()) {
                if (readbat.size() == 3) {

                    uint8_t byte1 = readbat[0];
                    uint8_t byte2 = readbat[1];

                    double voltage = (byte1 >>7 ) * 255 + byte2;
                    voltage = voltage/100;

                   // 显示十进制值
                    batdecimal = QString("%1V").arg(voltage,0,'f',2);
                    ui->barrtey->setFont(font);
                    ui->barrtey->setText(batdecimal);
                    ui->barrtey->setAlignment(Qt::AlignCenter);
                    QMessageBox::information(this, "电池电压读取", "电池电压读取成功！");
                }
            }

        }else if (text=="SD8840"||text=="SD8825"||text=="SD8931"||text=="SD8810"||text=="SD2010"||text=="SD2058"||text=="SD2059"||text=="SD2068"||text=="SD2069"||text=="SD2077"||text=="SD2078"||text=="SD2400"||text=="SD2401"||text=="SD2402"||text=="SD2403"||text=="SD2404"||text=="SD2405"||text=="SD8563"||\
                  text=="SD2200"||text=="SD2201"||text=="SD2202"||text=="SD2203"||text=="SD2204") {
            QMessageBox::information(this, "电池电压读取", "当前所选型号没有此功能！");
        }else if (text=="SD8800") {
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x1D);
            serialPort->write(hexData);

            QEventLoop loop;
            QObject::connect(serialPort, &QSerialPort::readyRead, &loop, &QEventLoop::quit);
            loop.exec();

            QByteArray readbat=serialPort->readAll();
            qDebug()<<"收到数据："<<readbat;
            qDebug() <<readbat.size();

            QString batdecimal;
            if (!readbat.isEmpty()) {
                if (readbat.size() == 3) {

                    uint8_t byte1 = readbat[0];
                    uint8_t byte2 = readbat[1];

                    double voltage = (byte1 >>7 ) * 255 + byte2;
                    voltage = voltage/100;

                   // 显示十进制值
                    batdecimal = QString("%1V").arg(voltage,0,'f',2);
                    ui->barrtey->setFont(font);
                    ui->barrtey->setText(batdecimal);
                    ui->barrtey->setAlignment(Qt::AlignCenter);
                    QMessageBox::information(this, "电池电压读取", "电池电压读取成功！");
                }
            }
        }else if (text=="SD8200") {
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x2A);
            serialPort->write(hexData);

            QEventLoop loop;
            QObject::connect(serialPort, &QSerialPort::readyRead, &loop, &QEventLoop::quit);
            loop.exec();

            QByteArray readbat=serialPort->readAll();
            qDebug()<<"收到数据："<<readbat;
            qDebug() <<readbat.size();

            QString batdecimal;
            if (!readbat.isEmpty()) {
                if (readbat.size() == 3) {

                    uint8_t byte1 = readbat[0];
                    uint8_t byte2 = readbat[1];

                    double voltage = (byte1 >>7 ) * 255 + byte2;
                    voltage = voltage/100;

                   // 显示十进制值
                    batdecimal = QString("%1V").arg(voltage,0,'f',2);
                    ui->barrtey->setFont(font);
                    ui->barrtey->setText(batdecimal);
                    ui->barrtey->setAlignment(Qt::AlignCenter);
                    QMessageBox::information(this, "电池电压读取", "电池电压读取成功！");
                }
            }
        }else if (text=="SD8902") {
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x2E);
            serialPort->write(hexData);

            QEventLoop loop;
            QObject::connect(serialPort, &QSerialPort::readyRead, &loop, &QEventLoop::quit);
            loop.exec();

            QByteArray readbat=serialPort->readAll();
            qDebug()<<"收到数据："<<readbat;
            qDebug() <<readbat.size();

            QString batdecimal;
            if (!readbat.isEmpty()) {
                if (readbat.size() == 3) {

                    uint8_t byte1 = readbat[0];
                    uint8_t byte2 = readbat[1];

                    double voltage = (byte1 >>7 ) * 255 + byte2;
                    voltage = voltage/100;

                   // 显示十进制值
                    batdecimal = QString("%1V").arg(voltage,0,'f',2);
                    ui->barrtey->setFont(font);
                    ui->barrtey->setText(batdecimal);
                    ui->barrtey->setAlignment(Qt::AlignCenter);
                    QMessageBox::information(this, "电池电压读取", "电池电压读取成功！");
                }
            }
        }

         }else{

            QMessageBox::information(this,"错误","串口打开失败，串口未连接或者被占用!");

        }
}

/**
功能：读芯片的id码
修改：sjm
参数：-
返回值：-
**/
void RtcMessage::on_idnum_clicked()
{
    QString text = ui->rtctype->toPlainText();
    //清空输入缓冲区
    serialPort->clear(QSerialPort::Input);

     QFont font;
     font.setBold(true);
     font.setPointSize(15); // 设置字体大小为12

     if(serialPort->isOpen())
     {
         if(text=="SD3068"||text=="SD3010"||text=="SD3025"||text=="SD3031"||text=="SD3077"||text=="SD3078"||text=="SD3178"||text=="SD3900"||text=="SD2503"||text=="SD2505"||text=="SD2506"||text=="SD2507"||text=="SD2508"||text=="SD2520"||text=="SD2522"||text=="SD2523")
         {
             // 创建一个QByteArray来存储十六进制数据
             QByteArray hexData;
             hexData.append(0xBC);
             hexData.append(0xCD);
             hexData.append(0xF3);
             serialPort->write(hexData);

             QEventLoop loop;
             QObject::connect(serialPort, &QSerialPort::readyRead, &loop, &QEventLoop::quit);//串口等待芯片的id码数据发送
             loop.exec();
             //处理id码数据
             QByteArray readid = serialPort->readAll();
             qDebug()<<"收到数据："<<readid;
             qDebug() <<readid.size();
             if (!readid.isEmpty()){
                 if (readid.size() == 9) {
                     uint8_t id0 = ((readid[0] & 0xF0) >> 4) * 10 + (readid[0] & 0x0F);
                     uint8_t id1 = ((readid[1] & 0xF0) >> 4) * 10 + (readid[1] & 0x0F);
                     uint8_t id2 = ((readid[2] & 0xF0) >> 4) * 10 + (readid[2] & 0x0F);
                     uint8_t id3 = ((readid[3] & 0xF0) >> 4) * 10 + (readid[3] & 0x0F);
                     uint8_t id4 = ((readid[4] & 0xF0) >> 4) * 10 + (readid[4] & 0x0F);
                     uint8_t id5 = ((readid[5] & 0xF0) >> 4) * 10 + (readid[5] & 0x0F);
                     uint8_t id6 = ((readid[6] & 0xF0) >> 4) * 10 + (readid[6] & 0x0F);
                     uint8_t id7 = ((readid[7] & 0xF0) >> 4) * 10 + (readid[7] & 0x0F);
                     uint8_t id8 = ((readid[8] & 0xF0) >> 4) * 10 + (readid[8] & 0x0F);

                     // 转换为十进制
                     QString iddecima = QString("%1-%2-%3-%4-%5-%6-%7-%8")
                         .arg(id0, 2, 10, QChar('0'))
                         .arg(id1, 2, 10, QChar('0'))
                         .arg(id2, 2, 10, QChar('0'))
                         .arg(id3, 2, 10, QChar('0'))
                         .arg(id4, 2, 10, QChar('0'))
                         .arg(id5, 2, 10, QChar('0'))
                         .arg(id6, 2, 10, QChar('0'))
                         .arg(id7, 2, 10, QChar('0'))
                         .arg(id8, 2, 10, QChar('0'));

                      ui->id->setFont(font);
                      ui->id->setText(iddecima);
                      QMessageBox::information(this, "ID码读取", "ID码读取成功！");
                     }
             }

       }else if (text=="SD8810") {
             // 创建一个QByteArray来存储十六进制数据
             QByteArray hexData;
             hexData.append(0xBC);
             hexData.append(0xCD);
             hexData.append(0x24);
             serialPort->write(hexData);

             QEventLoop loop;
             QObject::connect(serialPort, &QSerialPort::readyRead, &loop, &QEventLoop::quit);
             loop.exec();

             QByteArray readid = serialPort->readAll();
             qDebug()<<"收到数据："<<readid;
             qDebug() <<readid.size();
             QString iddecima;
             if (!readid.isEmpty()){
                 if (readid.size() == 9) {
                     uint8_t id0 = ((readid[0] & 0xF0) >> 4) * 10 + (readid[0] & 0x0F);
                     uint8_t id1 = ((readid[1] & 0xF0) >> 4) * 10 + (readid[1] & 0x0F);
                     uint8_t id2 = ((readid[2] & 0xF0) >> 4) * 10 + (readid[2] & 0x0F);
                     uint8_t id3 = ((readid[3] & 0xF0) >> 4) * 10 + (readid[3] & 0x0F);
                     uint8_t id4 = ((readid[4] & 0xF0) >> 4) * 10 + (readid[4] & 0x0F);
                     uint8_t id5 = ((readid[5] & 0xF0) >> 4) * 10 + (readid[5] & 0x0F);
                     uint8_t id6 = ((readid[6] & 0xF0) >> 4) * 10 + (readid[6] & 0x0F);
                     uint8_t id7 = ((readid[7] & 0xF0) >> 4) * 10 + (readid[7] & 0x0F);
                     uint8_t id8 = ((readid[8] & 0xF0) >> 4) * 10 + (readid[8] & 0x0F);

                     // 转换为十进制
                     QString iddecima = QString("%1-%2-%3-%4-%5-%6-%7-%8")
                         .arg(id0, 2, 10, QChar('0'))
                         .arg(id1, 2, 10, QChar('0'))
                         .arg(id2, 2, 10, QChar('0'))
                         .arg(id3, 2, 10, QChar('0'))
                         .arg(id4, 2, 10, QChar('0'))
                         .arg(id5, 2, 10, QChar('0'))
                         .arg(id6, 2, 10, QChar('0'))
                         .arg(id7, 2, 10, QChar('0'))
                         .arg(id8, 2, 10, QChar('0'));

                      ui->id->setFont(font);
                      ui->id->setText(iddecima);
                      QMessageBox::information(this, "ID码读取", "ID码读取成功！");
                     }
             }
       }else if (text=="SD8939") {
             // 创建一个QByteArray来存储十六进制数据
             QByteArray hexData;
             hexData.append(0xBC);
             hexData.append(0xCD);
             hexData.append(0x34);
             serialPort->write(hexData);

             QEventLoop loop;
             QObject::connect(serialPort, &QSerialPort::readyRead, &loop, &QEventLoop::quit);
             loop.exec();

             QByteArray readid = serialPort->readAll();
             qDebug()<<"收到数据："<<readid;
             qDebug() <<readid.size();
             QString iddecima;
             if (!readid.isEmpty()){
                 if (readid.size() == 9) {
                     uint8_t id0 = ((readid[0] & 0xF0) >> 4) * 10 + (readid[0] & 0x0F);
                     uint8_t id1 = ((readid[1] & 0xF0) >> 4) * 10 + (readid[1] & 0x0F);
                     uint8_t id2 = ((readid[2] & 0xF0) >> 4) * 10 + (readid[2] & 0x0F);
                     uint8_t id3 = ((readid[3] & 0xF0) >> 4) * 10 + (readid[3] & 0x0F);
                     uint8_t id4 = ((readid[4] & 0xF0) >> 4) * 10 + (readid[4] & 0x0F);
                     uint8_t id5 = ((readid[5] & 0xF0) >> 4) * 10 + (readid[5] & 0x0F);
                     uint8_t id6 = ((readid[6] & 0xF0) >> 4) * 10 + (readid[6] & 0x0F);
                     uint8_t id7 = ((readid[7] & 0xF0) >> 4) * 10 + (readid[7] & 0x0F);
                     uint8_t id8 = ((readid[8] & 0xF0) >> 4) * 10 + (readid[8] & 0x0F);

                     // 转换为十进制
                     QString iddecima = QString("%1-%2-%3-%4-%5-%6-%7-%8")
                         .arg(id0, 2, 10, QChar('0'))
                         .arg(id1, 2, 10, QChar('0'))
                         .arg(id2, 2, 10, QChar('0'))
                         .arg(id3, 2, 10, QChar('0'))
                         .arg(id4, 2, 10, QChar('0'))
                         .arg(id5, 2, 10, QChar('0'))
                         .arg(id6, 2, 10, QChar('0'))
                         .arg(id7, 2, 10, QChar('0'))
                         .arg(id8, 2, 10, QChar('0'));

                      ui->id->setFont(font);
                      ui->id->setText(iddecima);
                      QMessageBox::information(this, "ID码读取", "ID码读取成功！");
                     }
            }
       }else if (text=="SD8800") {
             // 创建一个QByteArray来存储十六进制数据
             QByteArray hexData;
             hexData.append(0xBC);
             hexData.append(0xCD);
             hexData.append(0x46);
             serialPort->write(hexData);

             QEventLoop loop;
             QObject::connect(serialPort, &QSerialPort::readyRead, &loop, &QEventLoop::quit);
             loop.exec();

             QByteArray readid = serialPort->readAll();
             qDebug()<<"收到数据："<<readid;
             qDebug() <<readid.size();
             QString iddecima;
             if (!readid.isEmpty()){
                 if (readid.size() == 9) {
                     uint8_t id0 = ((readid[0] & 0xF0) >> 4) * 10 + (readid[0] & 0x0F);
                     uint8_t id1 = ((readid[1] & 0xF0) >> 4) * 10 + (readid[1] & 0x0F);
                     uint8_t id2 = ((readid[2] & 0xF0) >> 4) * 10 + (readid[2] & 0x0F);
                     uint8_t id3 = ((readid[3] & 0xF0) >> 4) * 10 + (readid[3] & 0x0F);
                     uint8_t id4 = ((readid[4] & 0xF0) >> 4) * 10 + (readid[4] & 0x0F);
                     uint8_t id5 = ((readid[5] & 0xF0) >> 4) * 10 + (readid[5] & 0x0F);
                     uint8_t id6 = ((readid[6] & 0xF0) >> 4) * 10 + (readid[6] & 0x0F);
                     uint8_t id7 = ((readid[7] & 0xF0) >> 4) * 10 + (readid[7] & 0x0F);
                     uint8_t id8 = ((readid[8] & 0xF0) >> 4) * 10 + (readid[8] & 0x0F);

                     // 转换为十进制
                     QString iddecima = QString("%1-%2-%3-%4-%5-%6-%7-%8")
                         .arg(id0, 2, 10, QChar('0'))
                         .arg(id1, 2, 10, QChar('0'))
                         .arg(id2, 2, 10, QChar('0'))
                         .arg(id3, 2, 10, QChar('0'))
                         .arg(id4, 2, 10, QChar('0'))
                         .arg(id5, 2, 10, QChar('0'))
                         .arg(id6, 2, 10, QChar('0'))
                         .arg(id7, 2, 10, QChar('0'))
                         .arg(id8, 2, 10, QChar('0'));

                      ui->id->setFont(font);
                      ui->id->setText(iddecima);
                      QMessageBox::information(this, "ID码读取", "ID码读取成功！");
                     }
            }
       }else if (text=="SD8931") {
             // 创建一个QByteArray来存储十六进制数据
             QByteArray hexData;
             hexData.append(0xBC);
             hexData.append(0xCD);
             hexData.append(0x58);
             serialPort->write(hexData);

             QEventLoop loop;
             QObject::connect(serialPort, &QSerialPort::readyRead, &loop, &QEventLoop::quit);
             loop.exec();

             QByteArray readid = serialPort->readAll();
             qDebug()<<"收到数据："<<readid;
             qDebug() <<readid.size();
             QString iddecima;
             if (!readid.isEmpty()){
                 if (readid.size() == 9) {
                     uint8_t id0 = ((readid[0] & 0xF0) >> 4) * 10 + (readid[0] & 0x0F);
                     uint8_t id1 = ((readid[1] & 0xF0) >> 4) * 10 + (readid[1] & 0x0F);
                     uint8_t id2 = ((readid[2] & 0xF0) >> 4) * 10 + (readid[2] & 0x0F);
                     uint8_t id3 = ((readid[3] & 0xF0) >> 4) * 10 + (readid[3] & 0x0F);
                     uint8_t id4 = ((readid[4] & 0xF0) >> 4) * 10 + (readid[4] & 0x0F);
                     uint8_t id5 = ((readid[5] & 0xF0) >> 4) * 10 + (readid[5] & 0x0F);
                     uint8_t id6 = ((readid[6] & 0xF0) >> 4) * 10 + (readid[6] & 0x0F);
                     uint8_t id7 = ((readid[7] & 0xF0) >> 4) * 10 + (readid[7] & 0x0F);
                     uint8_t id8 = ((readid[8] & 0xF0) >> 4) * 10 + (readid[8] & 0x0F);

                     // 转换为十进制
                     QString iddecima = QString("%1-%2-%3-%4-%5-%6-%7-%8")
                         .arg(id0, 2, 10, QChar('0'))
                         .arg(id1, 2, 10, QChar('0'))
                         .arg(id2, 2, 10, QChar('0'))
                         .arg(id3, 2, 10, QChar('0'))
                         .arg(id4, 2, 10, QChar('0'))
                         .arg(id5, 2, 10, QChar('0'))
                         .arg(id6, 2, 10, QChar('0'))
                         .arg(id7, 2, 10, QChar('0'))
                         .arg(id8, 2, 10, QChar('0'));

                      ui->id->setFont(font);
                      ui->id->setText(iddecima);
                      QMessageBox::information(this, "ID码读取", "ID码读取成功！");
                     }
            }
       }else if (text=="SD8825") {
             // 创建一个QByteArray来存储十六进制数据
             QByteArray hexData;
             hexData.append(0xBC);
             hexData.append(0xCD);
             hexData.append(0x68);
             serialPort->write(hexData);

             QEventLoop loop;
             QObject::connect(serialPort, &QSerialPort::readyRead, &loop, &QEventLoop::quit);
             loop.exec();

             QByteArray readid = serialPort->readAll();
             qDebug()<<"收到数据："<<readid;
             qDebug() <<readid.size();
             QString iddecima;
             if (!readid.isEmpty()){
                 if (readid.size() == 9) {
                     uint8_t id0 = ((readid[0] & 0xF0) >> 4) * 10 + (readid[0] & 0x0F);
                     uint8_t id1 = ((readid[1] & 0xF0) >> 4) * 10 + (readid[1] & 0x0F);
                     uint8_t id2 = ((readid[2] & 0xF0) >> 4) * 10 + (readid[2] & 0x0F);
                     uint8_t id3 = ((readid[3] & 0xF0) >> 4) * 10 + (readid[3] & 0x0F);
                     uint8_t id4 = ((readid[4] & 0xF0) >> 4) * 10 + (readid[4] & 0x0F);
                     uint8_t id5 = ((readid[5] & 0xF0) >> 4) * 10 + (readid[5] & 0x0F);
                     uint8_t id6 = ((readid[6] & 0xF0) >> 4) * 10 + (readid[6] & 0x0F);
                     uint8_t id7 = ((readid[7] & 0xF0) >> 4) * 10 + (readid[7] & 0x0F);
                     uint8_t id8 = ((readid[8] & 0xF0) >> 4) * 10 + (readid[8] & 0x0F);

                     // 转换为十进制
                     QString iddecima = QString("%1-%2-%3-%4-%5-%6-%7-%8")
                         .arg(id0, 2, 10, QChar('0'))
                         .arg(id1, 2, 10, QChar('0'))
                         .arg(id2, 2, 10, QChar('0'))
                         .arg(id3, 2, 10, QChar('0'))
                         .arg(id4, 2, 10, QChar('0'))
                         .arg(id5, 2, 10, QChar('0'))
                         .arg(id6, 2, 10, QChar('0'))
                         .arg(id7, 2, 10, QChar('0'))
                         .arg(id8, 2, 10, QChar('0'));

                      ui->id->setFont(font);
                      ui->id->setText(iddecima);
                      QMessageBox::information(this, "ID码读取", "ID码读取成功！");
                     }
            }
       }else if (text=="SD8200") {
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x78);
            serialPort->write(hexData);

            QEventLoop loop;
            QObject::connect(serialPort, &QSerialPort::readyRead, &loop, &QEventLoop::quit);
            loop.exec();

            QByteArray readid = serialPort->readAll();
            qDebug()<<"收到数据："<<readid;
            qDebug() <<readid.size();
            QString iddecima;
            if (!readid.isEmpty()){
                if (readid.size() == 9) {
                    uint8_t id0 = ((readid[0] & 0xF0) >> 4) * 10 + (readid[0] & 0x0F);
                    uint8_t id1 = ((readid[1] & 0xF0) >> 4) * 10 + (readid[1] & 0x0F);
                    uint8_t id2 = ((readid[2] & 0xF0) >> 4) * 10 + (readid[2] & 0x0F);
                    uint8_t id3 = ((readid[3] & 0xF0) >> 4) * 10 + (readid[3] & 0x0F);
                    uint8_t id4 = ((readid[4] & 0xF0) >> 4) * 10 + (readid[4] & 0x0F);
                    uint8_t id5 = ((readid[5] & 0xF0) >> 4) * 10 + (readid[5] & 0x0F);
                    uint8_t id6 = ((readid[6] & 0xF0) >> 4) * 10 + (readid[6] & 0x0F);
                    uint8_t id7 = ((readid[7] & 0xF0) >> 4) * 10 + (readid[7] & 0x0F);
                    uint8_t id8 = ((readid[8] & 0xF0) >> 4) * 10 + (readid[8] & 0x0F);

                    // 转换为十进制
                    QString iddecima = QString("%1-%2-%3-%4-%5-%6-%7-%8")
                        .arg(id0, 2, 10, QChar('0'))
                        .arg(id1, 2, 10, QChar('0'))
                        .arg(id2, 2, 10, QChar('0'))
                        .arg(id3, 2, 10, QChar('0'))
                        .arg(id4, 2, 10, QChar('0'))
                        .arg(id5, 2, 10, QChar('0'))
                        .arg(id6, 2, 10, QChar('0'))
                        .arg(id7, 2, 10, QChar('0'))
                        .arg(id8, 2, 10, QChar('0'));

                     ui->id->setFont(font);
                     ui->id->setText(iddecima);
                     QMessageBox::information(this, "ID码读取", "ID码读取成功！");
                    }
           }
       }else if (text=="SD8804") {
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x88);
            serialPort->write(hexData);

            QEventLoop loop;
            QObject::connect(serialPort, &QSerialPort::readyRead, &loop, &QEventLoop::quit);
            loop.exec();

            QByteArray readid = serialPort->readAll();
            qDebug()<<"收到数据："<<readid;
            qDebug() <<readid.size();
            QString iddecima;
            if (!readid.isEmpty()){
                if (readid.size() == 9) {
                    uint8_t id0 = ((readid[0] & 0xF0) >> 4) * 10 + (readid[0] & 0x0F);
                    uint8_t id1 = ((readid[1] & 0xF0) >> 4) * 10 + (readid[1] & 0x0F);
                    uint8_t id2 = ((readid[2] & 0xF0) >> 4) * 10 + (readid[2] & 0x0F);
                    uint8_t id3 = ((readid[3] & 0xF0) >> 4) * 10 + (readid[3] & 0x0F);
                    uint8_t id4 = ((readid[4] & 0xF0) >> 4) * 10 + (readid[4] & 0x0F);
                    uint8_t id5 = ((readid[5] & 0xF0) >> 4) * 10 + (readid[5] & 0x0F);
                    uint8_t id6 = ((readid[6] & 0xF0) >> 4) * 10 + (readid[6] & 0x0F);
                    uint8_t id7 = ((readid[7] & 0xF0) >> 4) * 10 + (readid[7] & 0x0F);
                    uint8_t id8 = ((readid[8] & 0xF0) >> 4) * 10 + (readid[8] & 0x0F);

                    // 转换为十进制
                    QString iddecima = QString("%1-%2-%3-%4-%5-%6-%7-%8")
                        .arg(id0, 2, 10, QChar('0'))
                        .arg(id1, 2, 10, QChar('0'))
                        .arg(id2, 2, 10, QChar('0'))
                        .arg(id3, 2, 10, QChar('0'))
                        .arg(id4, 2, 10, QChar('0'))
                        .arg(id5, 2, 10, QChar('0'))
                        .arg(id6, 2, 10, QChar('0'))
                        .arg(id7, 2, 10, QChar('0'))
                        .arg(id8, 2, 10, QChar('0'));

                     ui->id->setFont(font);
                     ui->id->setText(iddecima);
                     QMessageBox::information(this, "ID码读取", "ID码读取成功！");
                    }
           }
       }else if (text=="SD8902") {
            // 创建一个QByteArray来存储十六进制数据
            QByteArray hexData;
            hexData.append(0xBC);
            hexData.append(0xCD);
            hexData.append(0x4E);
            serialPort->write(hexData);

            QEventLoop loop;
            QObject::connect(serialPort, &QSerialPort::readyRead, &loop, &QEventLoop::quit);
            loop.exec();

            QByteArray readid = serialPort->readAll();
            qDebug()<<"收到数据："<<readid;
            qDebug() <<readid.size();
            QString iddecima;
            if (!readid.isEmpty()){
                if (readid.size() == 9) {
                    uint8_t id0 = ((readid[0] & 0xF0) >> 4) * 10 + (readid[0] & 0x0F);
                    uint8_t id1 = ((readid[1] & 0xF0) >> 4) * 10 + (readid[1] & 0x0F);
                    uint8_t id2 = ((readid[2] & 0xF0) >> 4) * 10 + (readid[2] & 0x0F);
                    uint8_t id3 = ((readid[3] & 0xF0) >> 4) * 10 + (readid[3] & 0x0F);
                    uint8_t id4 = ((readid[4] & 0xF0) >> 4) * 10 + (readid[4] & 0x0F);
                    uint8_t id5 = ((readid[5] & 0xF0) >> 4) * 10 + (readid[5] & 0x0F);
                    uint8_t id6 = ((readid[6] & 0xF0) >> 4) * 10 + (readid[6] & 0x0F);
                    uint8_t id7 = ((readid[7] & 0xF0) >> 4) * 10 + (readid[7] & 0x0F);
                    uint8_t id8 = ((readid[8] & 0xF0) >> 4) * 10 + (readid[8] & 0x0F);

                    // 转换为十进制
                    QString iddecima = QString("%1-%2-%3-%4-%5-%6-%7-%8")
                        .arg(id0, 2, 10, QChar('0'))
                        .arg(id1, 2, 10, QChar('0'))
                        .arg(id2, 2, 10, QChar('0'))
                        .arg(id3, 2, 10, QChar('0'))
                        .arg(id4, 2, 10, QChar('0'))
                        .arg(id5, 2, 10, QChar('0'))
                        .arg(id6, 2, 10, QChar('0'))
                        .arg(id7, 2, 10, QChar('0'))
                        .arg(id8, 2, 10, QChar('0'));

                     ui->id->setFont(font);
                     ui->id->setText(iddecima);
                     QMessageBox::information(this, "ID码读取", "ID码读取成功！");
                    }
           }
       }else if (text=="SD8840"||text=="SD2010"||text=="SD2058"||text=="SD2059"||text=="SD2068"||text=="SD2069"||text=="SD2077"||text=="SD2078"||text=="SD2400"||text=="SD2401"||text=="SD2402"||text=="SD2403"||text=="SD2404"||text=="SD2405"||text=="SD8563"||\
                 text=="SD2200"||text=="SD2201"||text=="SD2202"||text=="SD2203"||text=="SD2204") {
         QMessageBox::information(this, "ID码读取", "当前所选型号无ID码！");
      }

      }else{

         QMessageBox::information(this,"错误","串口打开失败，串口未连接或者被占用!");

     }


}
