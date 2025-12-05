#ifndef RTCDATA_H
#define RTCDATA_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QSql>
#include <QStringList>
#include <QString>
#include <QSqlTableModel>
#include <QTableView>
#include <QStandardItemModel>
#include <QDateTime>


class RTCdata
{

public:
    RTCdata();
    void SD2010CreateDB();
    void SD2058CreateDB();
    void SD2059CreateDB();
    void SD2068CreateDB();
    void SD2069CreateDB();
    void SD2077CreateDB();
    void SD2078CreateDB();
    void SD2200CreateDB();

    void SD3068CreateDB();
    void SD8563CreateDB();
    void SD8825CreateDB();
    void SD8804CreateDB();
    void SD8840CreateDB();
    void SD8902CreateDB();
    void SD8931CreateDB();
    void SD8810CreateDB();
    void SD8939CreateDB();
    void SD8800CreateDB();
    void SD8200CreateDB();


};

#endif // SD3068DATA_H
