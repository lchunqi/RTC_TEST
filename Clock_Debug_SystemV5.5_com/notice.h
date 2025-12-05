#ifndef NOTICE_H
#define NOTICE_H

#include <QMainWindow>

namespace Ui {
class notice;
}

class notice : public QMainWindow
{
    Q_OBJECT

public:
    explicit notice(QWidget *parent = nullptr);
    ~notice();

private slots:
    void on_pushButton_clicked();

private:
    Ui::notice *ui;
};

#endif // NOTICE_H
