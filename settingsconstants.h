#ifndef SETTINGSCONSTANTS_H
#define SETTINGSCONSTANTS_H

#include <QString>
#include <QStringList>

class SettingsConstants{
public:
    static const QString lastDir;           //type = string
    static const QString denoise;           //type = bool
    static const QString compress;          //type = bool
    static const QString ocl;               //type = bool
    static const QString outputColor;       //type = int
    static const QString outputWB;          //type = int
    static const QString outputFormat;      //type = int
    static const QString x3fLocation;       //type = string
    static const QString exifToolsLocation; //type = string

    static const QStringList wbOptions;
    static const QStringList colorOptions;
    static const QStringList formatOptions;

    static const QString lastDirDefault;
    static const bool denoiseDefault;
    static const bool compressDefault;
    static const bool oclDefault;
    static const int outputColorDefault;
    static const int outputWBDefault;
    static const int outputFormatDefault;
    static const QString x3fLocationDefault;
    static const QString exifToolsLocationDefault;
};

#endif // SETTINGSCONSTANTS_H
