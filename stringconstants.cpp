#include "stringconstants.h"

const QString StringConstants::lastDir = "lastDir";
const QString StringConstants::compress = "compress";
const QString StringConstants::denoise = "denoise";
const QString StringConstants::ocl = "ocl";
const QString StringConstants::outputColor = "outputColor";
const QString StringConstants::outputFormat = "outputFormat";
const QString StringConstants::outputWB = "outputWB";
const QString StringConstants::x3fLocation = "x3fLocation";
const QString StringConstants::exifToolsLocation = "exifToolsLocation";

const QStringList StringConstants::wbOptions = QStringList()
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

const QStringList StringConstants::formatOptions = QStringList()
        << QString("DNG (default)")
        << QString("Embedded JPG")
        << QString("TIFF");

const QStringList StringConstants::colorOptions = QStringList()
        << QString("sRGB (default)")
        << QString("AdobeRGB")
        << QString("ProPhotoRGB")
        << QString("None");
