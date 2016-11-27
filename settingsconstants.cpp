#include "settingsconstants.h"
#include <QDir>
#include <QMessageBox>
#include <QSettings>

const QString SettingsConstants::lastDir = "lastDir";
const QString SettingsConstants::compress = "compress";
const QString SettingsConstants::denoise = "denoise";
const QString SettingsConstants::ocl = "ocl";
const QString SettingsConstants::outputColor = "outputColor";
const QString SettingsConstants::outputFormat = "outputFormat";
const QString SettingsConstants::outputWB = "outputWB";
const QString SettingsConstants::x3fLocation = "x3fLocation";
const QString SettingsConstants::exifToolsLocation = "exifToolsLocation";

const QStringList SettingsConstants::wbOptions = QStringList()
        << QString("As Taken (Default)")
        << QString("Auto")
        << QString("Sunlight")
        << QString("Shadow")
        << QString("Overcast")
        << QString("Incandescent")
        << QString("Fluorescent")
        << QString("Flash")
        << QString("Custom")
        << QString("Color Temp")
        << QString("Auto LSP");

const QStringList SettingsConstants::formatOptions = QStringList()
        << QString("DNG (default)")
        << QString("Embedded JPG")
        << QString("TIFF");

const QStringList SettingsConstants::colorOptions = QStringList()
        << QString("sRGB (default)")
        << QString("AdobeRGB")
        << QString("ProPhotoRGB")
        << QString("None");

const QString SettingsConstants::lastDirDefault = QDir::currentPath();
const bool SettingsConstants::denoiseDefault = true;
const bool SettingsConstants::compressDefault = true;
const bool SettingsConstants::oclDefault = false;
const int SettingsConstants::outputColorDefault = 0;
const int SettingsConstants::outputWBDefault = 0;
const int SettingsConstants::outputFormatDefault = 0;
const QString SettingsConstants::x3fLocationDefault =
        QDir::currentPath() + "/x3f_extract.exe";  // only for windows!
const QString SettingsConstants::exifToolsLocationDefault = "";


bool SettingsConstants::checkSettings(){
    QSettings *settings = new QSettings;
    // use this function to check to see if the settings are properly configured
    QString programLocation = settings->value(SettingsConstants::x3fLocation).toString();
    if (programLocation.length() < 1){
        QMessageBox::critical(NULL, "Please set the location of the x3f extractor executable",
                              "This program needs the x3f_extract program to be installed.  You can get it here: <a href=\"https://github.com/Kalpanika/x3f/releases\">https://github.com/Kalpanika/x3f/releases</a>");
        return false;
    }
    QFileInfo info(programLocation);
    if (!info.isExecutable()){
        QMessageBox::critical(NULL, "The X3F extractor is not an executable.",
                              "The file chosen to be the X3F extractor is not an executable, so this extraction will fail.");
        return false;
    }
    programLocation = settings->value(SettingsConstants::exifToolsLocation).toString();
    if (programLocation.length() < 1){
        QMessageBox::critical(NULL, "Please set the location of the exiftools executable",
                              "This program uses the exif tools executable to copy metadata to the final file.  If you don't have it installed, metadata is not copied.  You can get it here: <a href=\"http://www.sno.phy.queensu.ca/~phil/exiftool/\">http://www.sno.phy.queensu.ca/~phil/exiftool/</a>");
        return true;  // so that we don't check the exif tool
    }
    QFileInfo info2(programLocation);
    if (!info2.isExecutable()){
        QMessageBox::critical(NULL, "The exif tool is not an executable.",
                              "The file chosen to be the exif tool is not an executable, so metadata copying will be skipped.");
    }
#ifdef Q_OS_WIN
    if (programLocation.contains("(-k)")){
        QMessageBox::critical(NULL, "The exif tools file name must be edited",
                              "The Windows exiftools executable name needs to be changed to exiftools.exe (ie, remove the '(-k)').  If you do not, the conversion will fail.");
        return false;  // so that we don't check the exif tool
    }
#endif
    return true;
}
