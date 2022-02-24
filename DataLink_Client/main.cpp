#include "mainwidget.h"

#include <QApplication>
#include <QMetaType>

int main(int argc, char *argv[])
{
    qRegisterMetaType<QVector<int> >
            ("QVector<int>");
    QApplication a(argc, argv);
    MainWidget w;
    w.show();
    return a.exec();
}
