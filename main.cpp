#include "mainwindow.h"
#include <QApplication>
#include <QSettings>
#include "settingsconstants.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("Kalpanika");
    QCoreApplication::setOrganizationDomain("kalpanika.com");
    QCoreApplication::setApplicationName("X3F Wrapper");

#if defined(Q_OS_DARWIN) || defined(Q_OS_DARWIN64) || defined(Q_OS_MAC) || defined(Q_OS_MAC64) || defined(Q_OS_MACX) || defined(Q_OS_OSX)
    SettingsConstants::x3fLocationDefault =
            QCoreApplication::applicationDirPath() + "/x3f_extract_mac";
    SettingsConstants::dngOpcodeLocationDefault =
            QCoreApplication::applicationDirPath() + "/opcodes";
#endif  // the mac needs the path to be set here.  Perhaps other platforms need it as well...

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
        settings.setValue(SettingsConstants::dngOpcodeLocation,
                          SettingsConstants::dngOpcodeLocationDefault);
        settings.sync();
    }

    MainWindow w;
    w.show();
    return a.exec();
}
