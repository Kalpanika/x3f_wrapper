#ifndef STRINGCONSTANTS_H
#define STRINGCONSTANTS_H

#include <QString>
#include <QStringList>

class StringConstants{
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
};

#endif // STRINGCONSTANTS_H
