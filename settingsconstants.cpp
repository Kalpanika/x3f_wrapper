#include "settingsconstants.h"
#include <QDir>

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
const QString SettingsConstants::x3fLocationDefault = QDir::currentPath();
const QString SettingsConstants::exifToolsLocationDefault = "";
