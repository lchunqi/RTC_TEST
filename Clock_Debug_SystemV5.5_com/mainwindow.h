#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "selectdevice.h"
#include <QPalette>
#include <QBrush>
#include <QPixmap>
#include <QResizeEvent>
#include <QLabel>



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT



public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    SelectDevice *Selectui;

private slots:
    void enter();

private:
    Ui::MainWindow *ui;



};
#endif // MAINWINDOW_H
