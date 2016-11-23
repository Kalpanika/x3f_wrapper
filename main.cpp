#include "mainwindow.h"
#include <QApplication>
#include <QSettings>
#include "settingsconstants.h"


int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("Kalpanika");
    QCoreApplication::setOrganizationDomain("kalpanika.com");
    QCoreApplication::setApplicationName("X3F Wrapper");

    QSettings settings;
    int dirLen = settings.value(SettingsConstants::lastDir).toString().length();
    if (!dirLen){
        //set up the denoising defaults
        settings.setValue(SettingsConstants::denoise,
                          SettingsConstants::denoiseDefault);
        settings.setValue(SettingsConstants::compress,
                          SettingsConstants::compressDefault);
        settings.setValue(SettingsConstants::ocl,
                          SettingsConstants::oclDefault);
        settings.setValue(SettingsConstants::outputFormat,
                          SettingsConstants::outputFormatDefault);
        settings.setValue(SettingsConstants::outputColor,
                          SettingsConstants::outputColorDefault);
        settings.setValue(SettingsConstants::outputWB,
                          SettingsConstants::outputWBDefault);
        settings.setValue(SettingsConstants::x3fLocation,
                          SettingsConstants::x3fLocationDefault);
        settings.setValue(SettingsConstants::exifToolsLocation,
                          SettingsConstants::exifToolsLocationDefault);
        settings.sync();
    }

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
