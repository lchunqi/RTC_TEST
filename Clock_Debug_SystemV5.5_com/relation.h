#ifndef RELATION_H
#define RELATION_H

#include <QMainWindow>
#include <QDesktopServices>
#include <QUrl>

namespace Ui {
class relation;
}

class relation : public QMainWindow
{
    Q_OBJECT

public:
    explicit relation(QWidget *parent = nullptr);
    ~relation();

private slots:
    void on_pushButton_clicked();

private:
    Ui::relation *ui;
};

#endif // RELATION_H
