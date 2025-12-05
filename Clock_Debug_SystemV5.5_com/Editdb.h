#ifndef EDITDB_H
#define EDITDB_H

#include <QItemDelegate>

class CustomItemDelegate : public QItemDelegate
{
public:
    CustomItemDelegate(QObject *parent = nullptr);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;


};

class CustomItemDelegate8825 : public QItemDelegate
{
public:
    CustomItemDelegate8825(QObject *parent = nullptr);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;


};

//8804
class CustomItemDelegate8804 : public QItemDelegate
{
public:
    CustomItemDelegate8804(QObject *parent = nullptr);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

//8800
class CustomItemDelegate8800 : public QItemDelegate
{
public:
    CustomItemDelegate8800(QObject *parent = nullptr);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};



#endif // EDITDB_H
