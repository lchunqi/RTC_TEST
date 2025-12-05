#ifndef SELECTDEVICE_H
#define SELECTDEVICE_H


#include "RTCdata.h"
#include "rtcmessage.h"
#include "ui_rtcmessage.h"
#include <QMainWindow>
#include <QListWidget>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QDebug>
#include <QDateTime>
#include <QString>
#include <QMessageBox>
#include <QByteArray>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QSql>
#include <QStringList>
#include <QTableView>





namespace Ui {
class SelectDevice;
}

class SelectDevice : public QMainWindow
{
    Q_OBJECT



public:
    explicit SelectDevice(QWidget *parent = nullptr);
    ~SelectDevice();
    Ui::SelectDevice *ui;
//    RTCdata *SD3068;
    void selcetrtcmess(void);



private slots:
    void listview1Click(const QItemSelection &selected, const QItemSelection &deselected);

    void listview2Click(const QItemSelection &selected, const QItemSelection &deselected);

    void listview3Click(const QItemSelection &selected, const QItemSelection &deselected);

    void onListViewClicked();

    void on_quit_clicked();


private:
//    Ui::SelectDevice *ui;


};

#endif // SELECTDEVICE_H
