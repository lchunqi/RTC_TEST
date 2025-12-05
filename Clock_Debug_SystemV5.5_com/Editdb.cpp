#include "Editdb.h"

/**
功能：自定义类--限制数据库表格编辑范围(此文件中的所有类函数功能一致)
修改：sjm
参数：QWidget* parent-创建编辑器控件的父窗口, const QStyleOptionViewItem& option-视图项的样式信息, const QModelIndex& index-模型索引

例如：
row()--行索引
column()--列索引
检查行索引是否小于等于63且为奇数行和列大于等于3小于等于11同时检查行索引是否大于等于64和列大于等于3小于等于11的范围可以编辑
if (((index.row() <= 63 && index.row() % 2 != 0) && (index.column() >= 3 && index.column() <= 11)) || ((index.row() >= 64) && (index.column() >= 3 && index.column() <= 11)))
{
        return QItemDelegate::createEditor(parent, option, index);//设置为可编辑
    } else {
        return nullptr;//设置为不可编辑
 }

返回值：QItemDelegate::createEditor(parent, option, index)-返回可编辑区域
**/
//3068,8563编辑状态
CustomItemDelegate::CustomItemDelegate(QObject *parent)
    : QItemDelegate(parent)
{

}

QWidget* CustomItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (((index.row() <= 63 && index.row() % 2 != 0) && (index.column() >= 3 && index.column() <= 11)) || ((index.row() >= 64) && (index.column() >= 3 && index.column() <= 11))) { // 检查行索引是否小于60且为奇数
             return QItemDelegate::createEditor(parent, option, index);

    } else {
        return nullptr; // 设置为不可编辑
    }
}


//8825,8902,8931,8810,8939编辑状态
CustomItemDelegate8825::CustomItemDelegate8825(QObject *parent)
    : QItemDelegate(parent)
{

}

QWidget *CustomItemDelegate8825::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
        if ((index.row() <= 500 && index.row() % 2 != 0) && (index.column() >= 3 && index.column() <= 11))
        {
            return QItemDelegate::createEditor(parent, option, index);

        }else{
            return nullptr; // 设置为不可编辑
        }

}

//8804
CustomItemDelegate8804::CustomItemDelegate8804(QObject *parent)
    : QItemDelegate(parent)
{

}

QWidget *CustomItemDelegate8804::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (((index.row() <= 79 && index.row() % 2 != 0) && (index.column() >= 3 && index.column() <= 11)) || ((index.row() >= 80 && index.row() <= 188) && (index.column() >= 3 && index.column() <= 11)) || ((index.row() >= 190 && index.row() % 2 == 0) && (index.column() >= 3 && index.column() <= 11))) { // 检查行索引是否小于60且为奇数
             return QItemDelegate::createEditor(parent, option, index);

    } else {
        return nullptr; // 设置为不可编辑
    }
}


//8800
CustomItemDelegate8800::CustomItemDelegate8800(QObject *parent)
    : QItemDelegate(parent)
{

}

QWidget *CustomItemDelegate8800::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (((index.row() <= 53 && index.row() % 2 != 0) && (index.column() >= 3 && index.column() <= 11)) || ((index.row() >= 54 && index.row() <= 162) && (index.column() >= 3 && index.column() <= 11)) || ((index.row() >= 163 && index.row() % 2 == 0) && (index.column() >= 3 && index.column() <= 11))) { // 检查行索引是否小于60且为奇数
             return QItemDelegate::createEditor(parent, option, index);

    } else {
        return nullptr; // 设置为不可编辑
    }
}
