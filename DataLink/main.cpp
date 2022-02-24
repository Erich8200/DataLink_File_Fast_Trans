#include "mainwidget.h"

#include <QApplication>
#include <QMetaType>
#include "my_global.h"

int main(int argc, char *argv[])
{
    g_file_trans_task_ID = 0;
    QApplication a(argc, argv);
    qRegisterMetaType<QVector<int> >
            ("QVector<int>");
    qRegisterMetaType<QList<QPersistentModelIndex> >
            ("QList<QPersistentModelIndex>");
    qRegisterMetaType<QAbstractItemModel::LayoutChangeHint>
            ("QAbstractItemModel::LayoutChangeHint");
    MainWidget w;
    w.show();
    int ret = a.exec();
    return ret;
}
