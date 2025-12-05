#include "selectdevice.h"
#include "ui_selectdevice.h"
#include <QStringListModel>
#include <QStringList>
#include <QTextBrowser>
#include <QString>


/**
功能：器件选择界面窗口
修改：sjm
参数：QWidget* parent-创建编辑器控件的父窗口，QMainWindow(parent)-调用基类QMainWindow的构造函数，ui(new Ui::SelectDevice)-动态创建Ui::SelectDevice界面类实例
返回值：-
**/
SelectDevice::SelectDevice(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SelectDevice)
        {
        ui->setupUi(this);
        // 在 SelectDevice.cpp 中
        this->setWindowTitle(tr("选择型号"));
        this->setWindowIcon(QIcon(":/jpg/picture/3.jpg"));
        this->setWindowTitle(tr("器件选择"));

        // 连接列表项点击信号
        connect(ui->sure,  &QPushButton::clicked, this, &SelectDevice::onListViewClicked);

        // 设置第二个和第三个listView不可编辑
        ui->listView1->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->listView2->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->listView3->setEditTriggers(QAbstractItemView::NoEditTriggers);

        // 设置第一个listView的大类选项
        QStringList categories;
        categories << "SD2XXX系列" <<"SD22XX系列"<< "SD24XX系列"<<"SD25XX系列"<< "SD3XXX系列" << "SD85XX系列"<< "SD88XX系列"<< "SD8XXX系列";
        ui->listView1->setModel(new QStringListModel(categories));

        // 连接第一个listView的item点击事件
        connect(ui->listView1->selectionModel(), &QItemSelectionModel::selectionChanged, this, &SelectDevice::listview1Click);

        // 禁用按钮
        ui->sure->setEnabled(false);


}

/**
功能：析构函数释放内存和空间
修改：sjm
参数：-
返回值：-
**/
SelectDevice::~SelectDevice()
{
    delete ui;
}

/**
功能：listview1槽函数--选择芯片型号大类
修改：sjm
参数：const QItemSelection &selected-选中的项, const QItemSelection &deselected-选择的项
返回值：-
**/
void SelectDevice::listview1Click(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected);

    // 获取当前选中的大类
    QModelIndexList indexes = selected.indexes();
    if (indexes.isEmpty())
        return;
    // 选中转为文本信息
    QString category = indexes.first().data().toString();

    // 根据大类设置第二个listView的小类选项
    QStringList subcategories;
    if (category == "SD2XXX系列") {
        subcategories << "SD2010"<<"SD2058"<<"SD2059"<<"SD2068"<<"SD2069"<<"SD2077"<<"SD2078";
    } else if (category == "SD3XXX系列") {
        subcategories << "SD3010"<<"SD3025"<<"SD3031"<<"SD3068"<<"SD3077"<<"SD3078"<<"SD3178"<<"SD3900";
    } else if (category == "SD85XX系列") {
        subcategories << "SD8563"<<"SD8564"<<"SD8565"<<"SD8568"<<"SD8573"<<"SD8578"<<"SD8589";
    } else if (category == "SD88XX系列") {
        subcategories << "SD8800"<<"SD8804"<<"SD8810"<<"SD8825"<<"SD8830"<<"SD8840";
    } else if (category == "SD8XXX系列") {
        subcategories << "SD8190"<<"SD8198"<<"SD8200"<<"SD8208"<<"SD8902"<<"SD8908"<<"SD8931"<<"SD8939";
    } else if (category == "SD24XX系列") {
        subcategories << "SD2400"<<"SD2401"<<"SD2402"<<"SD2403"<<"SD2404"<<"SD2405";
    } else if (category == "SD25XX系列") {
        subcategories << "SD2503"<<"SD2505"<<"SD2506"<<"SD2507"<<"SD2508"<<"SD2520"<<"SD2522"<<"SD2523";
    } else if (category == "SD22XX系列") {
        subcategories << "SD2200"<<"SD2201"<<"SD2202"<<"SD2203"<<"SD2204";
    }
    // 将选中的小类添加到listview2控件中显示
    ui->listView2->setModel(new QStringListModel(subcategories));
    // 清空 listView3 的内容
    ui->listView3->setModel(nullptr);
    // 禁用按钮
    ui->sure->setEnabled(false);
    // 连接第二个listView的item点击事件
    connect(ui->listView2->selectionModel(), &QItemSelectionModel::selectionChanged, this, &SelectDevice::listview2Click);
}

/**
功能：listview2槽函数--选择芯片型号小类
修改：sjm
参数：const QItemSelection &selected-选中的项, const QItemSelection &deselected-选择的项
返回值：-
**/
void SelectDevice::listview2Click(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected);

    // 获取当前选中的小类
    QModelIndexList indexes = selected.indexes();
    if (indexes.isEmpty())
        return;
    // 选中转为文本信息
    QString subcategory = indexes.first().data().toString();

    // 根据小类设置第三个listView的具体型号
    QStringList items;
    if (subcategory == "SD2010") {
        items << "SD2010";
    } else if (subcategory == "SD2058"){
        items << "SD2058";
    } else if (subcategory == "SD2059"){
        items << "SD2059";
    } else if (subcategory == "SD2068"){
        items << "SD2068";
    } else if (subcategory == "SD2069"){
        items << "SD2069";
    } else if (subcategory == "SD2077"){
        items << "SD2077";
    } else if (subcategory == "SD2078"){
        items << "SD2078";
    } else if (subcategory == "SD3010"){
        items << "SD3010";
    } else if (subcategory == "SD3025"){
        items << "SD3025";
    } else if (subcategory == "SD3031"){
        items << "SD3031";
    } else if (subcategory == "SD3068"){
        items << "SD3068";
    } else if (subcategory == "SD3077"){
        items << "SD3077";
    } else if (subcategory == "SD3078"){
        items << "SD3078";
    } else if (subcategory == "SD3178"){
        items << "SD3178";
    } else if (subcategory == "SD3900"){
        items << "SD3900";
    } else if (subcategory == "SD8563"){
        items << "SD8563";
    } else if (subcategory == "SD8564"){
        items << "SD8564";
    } else if (subcategory == "SD8565"){
        items << "SD8565";
    } else if (subcategory == "SD8568"){
        items << "SD8568";
    } else if (subcategory == "SD8573"){
        items << "SD8573";
    } else if (subcategory == "SD8578"){
        items << "SD8578";
    } else if (subcategory == "SD8589"){
        items << "SD8589";
    } else if (subcategory == "SD8589"){
        items << "SD8589";
    } else if (subcategory == "SD8800"){
        items << "SD8800";
    } else if (subcategory == "SD8804"){
        items << "SD8804";
    } else if (subcategory == "SD8810"){
        items << "SD8810";
    } else if (subcategory == "SD8825"){
        items << "SD8825";
    } else if (subcategory == "SD8830"){
        items << "SD8830";
    } else if (subcategory == "SD8840"){
        items << "SD8840";
    } else if (subcategory == "SD8190"){
        items << "SD8190";
    } else if (subcategory == "SD8198"){
        items << "SD8198";
    } else if (subcategory == "SD8200"){
        items << "SD8200";
    } else if (subcategory == "SD8208"){
        items << "SD8208";
    } else if (subcategory == "SD8902"){
        items << "SD8902";
    } else if (subcategory == "SD8908"){
        items << "SD8908";
    } else if (subcategory == "SD8931"){
        items << "SD8931";
    } else if (subcategory == "SD8939"){
        items << "SD8939";
    } else if (subcategory == "SD2400"){
        items << "SD2400";
    } else if (subcategory == "SD2401"){
        items << "SD2401";
    } else if (subcategory == "SD2402"){
        items << "SD2402";
    } else if (subcategory == "SD2403"){
        items << "SD2403";
    } else if (subcategory == "SD2404"){
        items << "SD2404";
    } else if (subcategory == "SD2405"){
        items << "SD2405";
    } else if (subcategory == "SD2503"){
        items << "SD2503";
    } else if (subcategory == "SD2505"){
        items << "SD2505";
    } else if (subcategory == "SD2506"){
        items << "SD2506";
    } else if (subcategory == "SD2507"){
        items << "SD2507";
    } else if (subcategory == "SD2508"){
        items << "SD2508";
    } else if (subcategory == "SD2520"){
        items << "SD2520";
    } else if (subcategory == "SD2522"){
        items << "SD2522";
    } else if (subcategory == "SD2523"){
        items << "SD2523";
    } else if (subcategory == "SD2200"){
        items << "SD2200";
    } else if (subcategory == "SD2201"){
        items << "SD2201";
    } else if (subcategory == "SD2202"){
        items << "SD2202";
    } else if (subcategory == "SD2203"){
        items << "SD2203";
    } else if (subcategory == "SD2204"){
        items << "SD2204";
    }
    // 将选中的小类添加到listview3控件中显示
    ui->listView3->setModel(new QStringListModel(items));
    // 禁用按钮
    ui->sure->setEnabled(false);
    // 连接第三个listView的item点击事件
    connect(ui->listView3->selectionModel(), &QItemSelectionModel::selectionChanged, this, &SelectDevice::listview3Click);
}

/**
功能：listview3槽函数--选择芯片具体型号
修改：sjm
参数：const QItemSelection &selected-选中的项, const QItemSelection &deselected-选择的项
返回值：-
**/
void SelectDevice::listview3Click(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected);

    // 获取当前选中的具体型号
    QModelIndexList indexes = selected.indexes();
    if (indexes.isEmpty())
        return;
    // 选中转为文本信息
    QString item = indexes.first().data().toString();

    if(item=="SD3068")
      {
        qDebug()<<"item："<<item;
    }

    if(item=="SD8563")
      {
        qDebug()<<"item："<<item;
    }

    // 开启按钮
    ui->sure->setEnabled(true);
    // 在textBrowser中显示选中的具体物品
    ui->textBrowser->setText(item);
}

/**
功能：退出按键关闭窗口槽函数
修改：sjm
参数：-
返回值：-
**/
void SelectDevice::on_quit_clicked()
{

    SelectDevice::close();

}

/**
功能：确定按键槽函数--选择型号同时设置信息
修改：sjm
参数：-
返回值：-
**/
void SelectDevice::onListViewClicked()
{
       // 获取当前选择项
       QModelIndex selectedIndex = ui->listView3->currentIndex();
       // 转为文本
       QString selectedModel = selectedIndex.data(Qt::DisplayRole).toString();

       // 创建对象并显示
       RtcMessage *rtc=new RtcMessage(selectedModel);
       rtc->show();
       this->hide();
       // 判断是否为 "sd2010"，如果是才显示属性
       if (selectedModel == "SD2010") {
           rtc->ui->rtctype->setText("SD2010");
           rtc->ui->rtcpack->setText("SOP8");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("无温补");
           rtc->ui->rtcbattery->setText("无");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD2010");
       }

       if (selectedModel == "SD2058") {
           rtc->ui->rtctype->setText("SD2058");
           rtc->ui->rtcpack->setText("SOP8");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("无温补");
           rtc->ui->rtcbattery->setText("无");
           rtc->ui->rtccrystal->setText("外置晶振");
           rtc->labe2->setText("型号：SD2058");
       }

       if (selectedModel == "SD2059") {
           rtc->ui->rtctype->setText("SD2059");
           rtc->ui->rtcpack->setText("SOP8");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("无温补");
           rtc->ui->rtcbattery->setText("无");
           rtc->ui->rtccrystal->setText("外置晶振");
           rtc->labe2->setText("型号：SD2059");
       }

       if (selectedModel == "SD2068") {
           rtc->ui->rtctype->setText("SD2068");
           rtc->ui->rtcpack->setText("SOP8");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("无温补");
           rtc->ui->rtcbattery->setText("无");
           rtc->ui->rtccrystal->setText("外置晶振");
           rtc->labe2->setText("型号：SD2068");
       }

       if (selectedModel == "SD2069") {
           rtc->ui->rtctype->setText("SD2069");
           rtc->ui->rtcpack->setText("SOP8");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("无温补");
           rtc->ui->rtcbattery->setText("无");
           rtc->ui->rtccrystal->setText("外置晶振");
           rtc->labe2->setText("型号：SD2069");
       }

       if (selectedModel == "SD2077") {
           rtc->ui->rtctype->setText("SD2077");
           rtc->ui->rtcpack->setText("SOP8");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("无温补");
           rtc->ui->rtcbattery->setText("无");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD2077");
       }

       if (selectedModel == "SD2078") {
           rtc->ui->rtctype->setText("SD2078");
           rtc->ui->rtcpack->setText("SOP8");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("无温补");
           rtc->ui->rtcbattery->setText("无");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD2078");
       }

       if (selectedModel == "SD3010") {
           rtc->ui->rtctype->setText("SD3010");
           rtc->ui->rtcpack->setText("SOP8");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("自动补偿");
           rtc->ui->rtcbattery->setText("无");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD3010");
           }

       if (selectedModel == "SD3025") {
           rtc->ui->rtctype->setText("SD3025");
           rtc->ui->rtcpack->setText("SOP14");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("自动补偿");
           rtc->ui->rtcbattery->setText("无");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD3025");
           }

       if (selectedModel == "SD3031") {
           rtc->ui->rtctype->setText("SD3031");
           rtc->ui->rtcpack->setText("SOP8");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("自动补偿");
           rtc->ui->rtcbattery->setText("无");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD3031");
           }

       if (selectedModel == "SD3068") {
           rtc->ui->rtctype->setText("SD3068");
           rtc->ui->rtcpack->setText("SOP8");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("自动补偿");
           rtc->ui->rtcbattery->setText("无");
           rtc->ui->rtccrystal->setText("外置晶振");
           rtc->labe2->setText("型号：SD3068");
           }

       if (selectedModel == "SD3077") {
           rtc->ui->rtctype->setText("SD3077");
           rtc->ui->rtcpack->setText("SOP8");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("自动补偿");
           rtc->ui->rtcbattery->setText("无");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD3077");
           }

       if (selectedModel == "SD3078") {
           rtc->ui->rtctype->setText("SD3078");
           rtc->ui->rtcpack->setText("SOP8");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("自动补偿");
           rtc->ui->rtcbattery->setText("无");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD3078");
           }

       if (selectedModel == "SD3178") {
           rtc->ui->rtctype->setText("SD3178");
           rtc->ui->rtcpack->setText("SOP8");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("自动补偿");
           rtc->ui->rtcbattery->setText("无");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD3178");
           }

       if (selectedModel == "SD3900") {
           rtc->ui->rtctype->setText("SD3900");
           rtc->ui->rtcpack->setText("3225");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("自动补偿");
           rtc->ui->rtcbattery->setText("无");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD3900");
           }

       if (selectedModel == "SD8563") {
           rtc->ui->rtctype->setText("SD8563");
           rtc->ui->rtcpack->setText("SOP8");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("无温补");
           rtc->ui->rtcbattery->setText("无");
           rtc->ui->rtccrystal->setText("外置晶振");
           rtc->labe2->setText("型号：SD8563");
       }

       if (selectedModel == "SD8564") {
           rtc->ui->rtctype->setText("SD8564");
           rtc->ui->rtcpack->setText("3215");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("无温补");
           rtc->ui->rtcbattery->setText("无");
           rtc->ui->rtccrystal->setText("外置晶振");
           rtc->labe2->setText("型号：SD8564");
       }

       if (selectedModel == "SD8565") {
           rtc->ui->rtctype->setText("SD8565");
           rtc->ui->rtcpack->setText("2012");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("无温补");
           rtc->ui->rtcbattery->setText("无");
           rtc->ui->rtccrystal->setText("外置晶振");
           rtc->labe2->setText("型号：SD8565");
       }

       if (selectedModel == "SD8568") {
           rtc->ui->rtctype->setText("SD8568");
           rtc->ui->rtcpack->setText("SOP8");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("无温补");
           rtc->ui->rtcbattery->setText("无");
           rtc->ui->rtccrystal->setText("外置晶振");
           rtc->labe2->setText("型号：SD8568");
       }

       if (selectedModel == "SD8825") {
           rtc->ui->rtctype->setText("SD8825");
           rtc->ui->rtcpack->setText("SOP14");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("自动补偿");
           rtc->ui->rtcbattery->setText("无");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD8825");
       }

       if (selectedModel == "SD8840") {
           rtc->ui->rtctype->setText("SD8840");
           rtc->ui->rtcpack->setText("3225");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("自动补偿");
           rtc->ui->rtcbattery->setText("无");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD8840");
       }

       if (selectedModel == "SD8902") {
           rtc->ui->rtctype->setText("SD8902");
           rtc->ui->rtcpack->setText("SOP8");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("无");
           rtc->ui->rtcbattery->setText("无");
           rtc->ui->rtccrystal->setText("外置晶振");
           rtc->labe2->setText("型号：SD8902");
       }

       if (selectedModel == "SD8804") {
           rtc->ui->rtctype->setText("SD8804");
           rtc->ui->rtcpack->setText("3225");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("自动补偿");
           rtc->ui->rtcbattery->setText("无");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD8804");
       }

       if (selectedModel == "SD8939") {
           rtc->ui->rtctype->setText("SD8939");
           rtc->ui->rtcpack->setText("SOP16");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("自动补偿");
           rtc->ui->rtcbattery->setText("无");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD8939");
       }

       if (selectedModel == "SD8804") {
           rtc->ui->rtctype->setText("SD8804");
           rtc->ui->rtcpack->setText("3225");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("自动补偿");
           rtc->ui->rtcbattery->setText("无");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD8804");
       }

       if (selectedModel == "SD8825") {
           rtc->ui->rtctype->setText("SD8825");
           rtc->ui->rtcpack->setText("SOP14");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("自动补偿");
           rtc->ui->rtcbattery->setText("无");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD8825");
       }

       if (selectedModel == "SD8840") {
           rtc->ui->rtctype->setText("SD8840");
           rtc->ui->rtcpack->setText("3225");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("自动补偿");
           rtc->ui->rtcbattery->setText("无");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD8840");
       }

       if (selectedModel == "SD8810") {
           rtc->ui->rtctype->setText("SD8810");
           rtc->ui->rtcpack->setText("SOP8");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("自动补偿");
           rtc->ui->rtcbattery->setText("无");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD8810");
       }

       if (selectedModel == "SD8931") {
           rtc->ui->rtctype->setText("SD8931");
           rtc->ui->rtcpack->setText("SOP8");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("自动补偿");
           rtc->ui->rtcbattery->setText("无");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD8931");
       }

       if (selectedModel == "SD8902") {
           rtc->ui->rtctype->setText("SD8902");
           rtc->ui->rtcpack->setText("SOP8");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("无温补");
           rtc->ui->rtcbattery->setText("无");
           rtc->ui->rtccrystal->setText("外置晶振");
           rtc->labe2->setText("型号：SD8902");
       }

       if (selectedModel == "SD8800") {
           rtc->ui->rtctype->setText("SD8800");
           rtc->ui->rtcpack->setText("3225");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("自动补偿");
           rtc->ui->rtcbattery->setText("无");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD8800");
       }

       if (selectedModel == "SD8200") {
           rtc->ui->rtctype->setText("SD8200");
           rtc->ui->rtcpack->setText("SOP8");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("自动补偿");
           rtc->ui->rtcbattery->setText("无");
           rtc->ui->rtccrystal->setText("外置晶振");
           rtc->labe2->setText("型号：SD8200");
       }

       if (selectedModel == "SD2400") {
           rtc->ui->rtctype->setText("SD2400");
           rtc->ui->rtcpack->setText("DIP24");
           rtc->ui->rtcsize->setText("0~512kbit");
           rtc->ui->rtcrange->setText("无温补");
           rtc->ui->rtcbattery->setText("一次性电池");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD2400");
       }

       if (selectedModel == "SD2401") {
           rtc->ui->rtctype->setText("SD2401");
           rtc->ui->rtcpack->setText("DIP24");
           rtc->ui->rtcsize->setText("0~512kbit");
           rtc->ui->rtcrange->setText("无温补");
           rtc->ui->rtcbattery->setText("充电电池");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD2401");
       }

       if (selectedModel == "SD2402") {
           rtc->ui->rtctype->setText("SD2402");
           rtc->ui->rtcpack->setText("DIP16");
           rtc->ui->rtcsize->setText("0~512kbit");
           rtc->ui->rtcrange->setText("无温补");
           rtc->ui->rtcbattery->setText("无");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD2402");
       }

       if (selectedModel == "SD2403") {
           rtc->ui->rtctype->setText("SD2403");
           rtc->ui->rtcpack->setText("DIP8");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("无温补");
           rtc->ui->rtcbattery->setText("无");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD2403");
       }

       if (selectedModel == "SD2404") {
           rtc->ui->rtctype->setText("SD2404");
           rtc->ui->rtcpack->setText("DIP24");
           rtc->ui->rtcsize->setText("2kbit");
           rtc->ui->rtcrange->setText("自动补偿");
           rtc->ui->rtcbattery->setText("一次性电池");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD2404");
       }

       if (selectedModel == "SD2405") {
           rtc->ui->rtctype->setText("SD2405");
           rtc->ui->rtcpack->setText("DIP16");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("无温补");
           rtc->ui->rtcbattery->setText("充电电池");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD2405");
       }

       if (selectedModel == "SD2503") {
           rtc->ui->rtctype->setText("SD2503");
           rtc->ui->rtcpack->setText("DIP8");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("自动温补");
           rtc->ui->rtcbattery->setText("充电电池");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD2503");
       }

       if (selectedModel == "SD2505") {
           rtc->ui->rtctype->setText("SD2505");
           rtc->ui->rtcpack->setText("DIP16");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("自动温补");
           rtc->ui->rtcbattery->setText("充电电池");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD2505");
       }

       if (selectedModel == "SD2506") {
           rtc->ui->rtctype->setText("SD2506");
           rtc->ui->rtcpack->setText("DIP8");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("自动温补");
           rtc->ui->rtcbattery->setText("充电电池");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD2506");
       }

       if (selectedModel == "SD2507") {
           rtc->ui->rtctype->setText("SD2507");
           rtc->ui->rtcpack->setText("DIP16");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("自动温补");
           rtc->ui->rtcbattery->setText("充电电池");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD2507");
       }

       if (selectedModel == "SD2520") {
           rtc->ui->rtctype->setText("SD2520");
           rtc->ui->rtcpack->setText("DIP24");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("自动温补");
           rtc->ui->rtcbattery->setText("充电电池");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD2520");
       }

       if (selectedModel == "SD2522") {
           rtc->ui->rtctype->setText("SD2522");
           rtc->ui->rtcpack->setText("DIP24");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("自动温补");
           rtc->ui->rtcbattery->setText("充电电池");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD2522");
       }

       if (selectedModel == "SD2523") {
           rtc->ui->rtctype->setText("SD2523");
           rtc->ui->rtcpack->setText("SOP24");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("自动温补");
           rtc->ui->rtcbattery->setText("充电电池");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD2523");
       }

       if (selectedModel == "SD2508") {
           rtc->ui->rtctype->setText("SD2508");
           rtc->ui->rtcpack->setText("SOP10");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("自动温补");
           rtc->ui->rtcbattery->setText("充电电池");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD2503");
       }

       if (selectedModel == "SD2500") {
           rtc->ui->rtctype->setText("SD2500");
           rtc->ui->rtcpack->setText("DIP24");
           rtc->ui->rtcsize->setText("0~512kbit");
           rtc->ui->rtcrange->setText("自动温补");
           rtc->ui->rtcbattery->setText("一次性电池");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD2500");
       }

       if (selectedModel == "SD2200") {
           rtc->ui->rtctype->setText("SD2200");
           rtc->ui->rtcpack->setText("DIP24");
           rtc->ui->rtcsize->setText("0~512kbit");
           rtc->ui->rtcrange->setText("无");
           rtc->ui->rtcbattery->setText("一次性电池");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD2200");
       }

       if (selectedModel == "SD2201") {
           rtc->ui->rtctype->setText("SD2201");
           rtc->ui->rtcpack->setText("DIP24");
           rtc->ui->rtcsize->setText("0~512kbit");
           rtc->ui->rtcrange->setText("无");
           rtc->ui->rtcbattery->setText("充电电池");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD2201");
       }

       if (selectedModel == "SD2202") {
           rtc->ui->rtctype->setText("SD2202");
           rtc->ui->rtcpack->setText("DIP8/16");
           rtc->ui->rtcsize->setText("0~512kbit");
           rtc->ui->rtcrange->setText("无");
           rtc->ui->rtcbattery->setText("无");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD2202");
       }

       if (selectedModel == "SD2203") {
           rtc->ui->rtctype->setText("SD2203");
           rtc->ui->rtcpack->setText("DIP8");
           rtc->ui->rtcsize->setText("无");
           rtc->ui->rtcrange->setText("无");
           rtc->ui->rtcbattery->setText("无");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD2203");
       }

       if (selectedModel == "SD2204") {
           rtc->ui->rtctype->setText("SD2204");
           rtc->ui->rtcpack->setText("DIP24");
           rtc->ui->rtcsize->setText("2kbit");
           rtc->ui->rtcrange->setText("主动补偿");
           rtc->ui->rtcbattery->setText("一次性电池");
           rtc->ui->rtccrystal->setText("内置晶振");
           rtc->labe2->setText("型号：SD2204");
       }

}







