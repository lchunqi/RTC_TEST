#ifndef RTCMESSAGE_H
#define RTCMESSAGE_H

#include "selectdevice.h"
#include "ui_selectdevice.h"
#include "notice.h"
#include "ui_notice.h"
#include "relation.h"
#include "ui_relation.h"
#include "history.h"
#include "ui_history.h"

#include "RTCdata.h"
#include "Editdb.h"
#include <QMainWindow>
#include <QToolBar>
#include <QPushButton>
#include <QLabel>
#include <QMenuBar>
#include <QAction>
#include <QTimer>
#include <QDateTime>
#include <QString>
#include <QProcess>
#include <QMessageBox>
#include <QDebug>
#include <QByteArray>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QSql>
#include <QStringList>
#include <QSqlTableModel>
#include <QTableView>
#include <QSqlRecord>
#include <QThread>
#include <QDesktopServices>
#include <QUrl>
#include <algorithm>


namespace Ui {
class RtcMessage;
}

class RtcMessage : public QMainWindow
{
    Q_OBJECT

public:
    explicit RtcMessage(QString selectedModel,QWidget *parent = nullptr);
    ~RtcMessage();
    Ui::RtcMessage *ui;
    RTCdata *SD3068;
    RTCdata *SD8563;
    RTCdata *SD8825;
    RTCdata *SD8804;
    RTCdata *SD8840;
    RTCdata *SD8902;
    RTCdata *SD8931;
    RTCdata *SD8810;
    RTCdata *SD8939;
    RTCdata *SD8800;
    RTCdata *SD8200;
    RTCdata *SD2010;
    RTCdata *SD2058;
    RTCdata *SD2059;
    RTCdata *SD2068;
    RTCdata *SD2069;
    RTCdata *SD2077;
    RTCdata *SD2078;
    RTCdata *SD2200;


    QSqlTableModel *sd3068model;
    QSqlTableModel *sd8563model;
    QSqlTableModel *sd8825model;
    QSqlTableModel *sd8804model;
    QSqlTableModel *sd8840model;
    QSqlTableModel *sd8902model;
    QSqlTableModel *sd8931model;
    QSqlTableModel *sd8810model;
    QSqlTableModel *sd8939model;
    QSqlTableModel *sd8800model;
    QSqlTableModel *sd8200model;
    QSqlTableModel *sd2010model;
    QSqlTableModel *sd2058model;
    QSqlTableModel *sd2059model;
    QSqlTableModel *sd2068model;
    QSqlTableModel *sd2069model;
    QSqlTableModel *sd2077model;
    QSqlTableModel *sd2078model;
    QSqlTableModel *sd2200model;


    QLabel *label;
    QLabel *labe2;
    QLabel *labe4;
    QToolBar * toolbar;
    void tool(void);

    void sd3068(void);
    void sd3068Modelselt(void);//数据库模型显示
    void sd8563(void);
    void sd8563Modelselt(void);
    void sd8825(void);
    void sd8825Modelselt(void);
    void sd8804(void);
    void sd8804Modelselt(void);
    void sd8840(void);
    void sd8840Modelselt(void);
    void sd8902(void);
    void sd8902Modelselt(void);
    void sd8931(void);
    void sd8931Modelselt(void);
    void sd8810(void);
    void sd8810Modelselt(void);
    void sd8939(void);
    void sd8939Modelselt(void);
    void sd8800(void);
    void sd8800Modelselt(void);
    void sd8200(void);
    void sd8200Modelselt(void);
    void sd2010(void);
    void sd2010Modelselt(void);
    void sd2058(void);
    void sd2058Modelselt(void);
    void sd2059(void);
    void sd2059Modelselt(void);
    void sd2068(void);
    void sd2068Modelselt(void);
    void sd2069(void);
    void sd2069Modelselt(void);
    void sd2077(void);
    void sd2077Modelselt(void);
    void sd2078(void);
    void sd2078Modelselt(void);
    void sd2200(void);
    void sd2200Modelselt(void);

private slots:
    void on_refresh_clicked();//刷新

    void on_save_clicked();//保存

    void updateSerialData();//更新串口

    void changeSerialPort(const QString& portName);//切换串口

    void on_CCS_clicked();//校准时钟源

    void on_readtime_clicked();//读时间 1

    void on_R00H_clicked();//读00H 2

    void on_R20H_clicked();//读20H 3

    void on_R40H_clicked();//读40H 4

    void on_R60H_clicked();//读60H 5

    void on_writetime_clicked();//写时间 1-1

    void on_W00H_clicked();//写00H 2-2

    void on_W07H_clicked();//写07H 3-3

    void on_W2CH_clicked();//写2CH 4-4

    void on_W40H_clicked();//写40H 5-5

    void on_W60H_clicked();//写60H 6-6

    void on_idnum_clicked();//读id码

    void on_tempButton_clicked();//读温度

    void on_batButton_clicked();//读电压

    void skip();

    void notices();

    void homepages();

    void relations();

    void historys();

    void Computer_time();

    void ReCom(void);


private:
//    Ui::RtcMessage *ui;
    QAction *Close;
    QAction *Device;
    QAction *Help;
    QTimer *timer;
    QTimer *timerSerial;
    void Serialinit(void);

};

#endif // RTCMESSAGE_H
