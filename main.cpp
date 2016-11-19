#include "mainwindow.h"
#include <QApplication>
#include <QSettings>
#include "stringconstants.h"


int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("Kalpanika");
    QCoreApplication::setOrganizationDomain("kalpanika.com");
    QCoreApplication::setApplicationName("X3F Wrapper");

    QSettings settings;
    int dirLen = settings.value(StringConstants::lastDir).toString().length();
    if (!dirLen){
        //set up the denoising defaults
        settings.setValue(StringConstants::denoise, true);
        settings.setValue(StringConstants::compress, true);
        settings.setValue(StringConstants::ocl, true);
        settings.setValue(StringConstants::outputFormat, 0);
        settings.setValue(StringConstants::outputColor, 0);
        settings.setValue(StringConstants::outputWB, 0);
        settings.setValue(StringConstants::x3fLocation, "");
        settings.setValue(StringConstants::exifToolsLocation, "");
        settings.sync();
    }

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
