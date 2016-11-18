#include "mainwindow.h"
#include <QApplication>



int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("Kalpanika");
    QCoreApplication::setOrganizationDomain("kalpanika.com");
    QCoreApplication::setApplicationName("X3F Wrapper");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
