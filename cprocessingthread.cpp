#include "cprocessingthread.h"
#include <QSettings>
#include "stringconstants.h"
#include <QProcess>


// see https://forum.qt.io/topic/60265/separate-qthread-to-update-qdialog-s-ui/3

CProcessingThread::CProcessingThread(const QStringList& inFiles,
                                     const QDir& inCurrentDir)
{
    settings = new QSettings;
    files = inFiles;
    currentDir = inCurrentDir;
}

QStringList CProcessingThread::buildArgList(){
    QStringList output;
    if (!settings->value(StringConstants::denoise).toBool()){
        output << "-no-denoise";
    }
    if (settings->value(StringConstants::compress).toBool()){
        output << "-compress";
    }
    if (settings->value(StringConstants::ocl).toBool()){
        output << "-ocl";
    }
    // for the color, etc-- assume 0 is the default
    int val = settings->value(StringConstants::outputColor).toInt();
    if (val > 0){
        output << "-color";
        output << StringConstants::colorOptions[val];
    }
    val = settings->value(StringConstants::outputFormat).toInt();
    switch(val){
    case 1:  // "Embedded JPG"
        output << "-jpg";
        break;
    case 2:  // "TIFF"
        output << "-tiff";
        break;
    default:  // "DNG", ie, do nothing
        break;
    }
    val = settings->value(StringConstants::outputWB).toInt();
    if (val > 0){
        output << "-wb"; // We sure this is the right way to set up the WB call?
        output << StringConstants::wbOptions[val];
    }
    return output;
}


void CProcessingThread::convertX3FFile(const QUrl& fileName, const QStringList& inArgs){
    if (QFile::exists(fileName.toLocalFile() + ".dng")){
        return;
    }
    QString tmpFileName = fileName.toLocalFile() + ".dng.tmp";
    if (QFile::exists(tmpFileName)){
        bool removalSuccessful = QFile::remove(tmpFileName);
        if (!removalSuccessful){
            emit error_message("A temp file exists and needs to be deleted.",
                               "A file named " + tmpFileName + " exists and needs to be deleted before processing can continue.");
            return;
        }
    }

    QString converter = settings->value(StringConstants::x3fLocation).toString();

    QStringList arguments = inArgs;
    arguments << fileName.toLocalFile();
    int exitCode = QProcess::execute(converter, arguments);
    if (exitCode != 0){
        emit error_message("Something went wrong.",
                           "Something happened while processing the image.  Error code: " + QString::number(exitCode) +
                           " argument list: [" + arguments.join(", ") + "]");

        return;
    }

    QString exiftools = settings->value(StringConstants::exifToolsLocation).toString();
    QStringList exiftoolsargs;
    exiftoolsargs << exiftools;
    exiftoolsargs << "-TagsFromFile";
    exiftoolsargs << fileName.toLocalFile();
    exiftoolsargs << "-all:all";
    int format = settings->value(StringConstants::outputFormat).toInt();
    switch(format){
    case 1:
        //immaterial for jpgs
        break;
    case 2:
        exiftoolsargs << fileName.toLocalFile() + ".tiff";
        exitCode = QProcess::execute(exiftools, exiftoolsargs);
        break;
    default:
        exiftoolsargs << fileName.toLocalFile() + ".dng";
        exitCode = QProcess::execute(exiftools, exiftoolsargs);
    }
    //qDebug() << exiftoolsargs;
}

void CProcessingThread::run()
{
    QStringList arguments = buildArgList();
    for (int i = 0; i < files.size(); ++i) {
        convertX3FFile(QUrl::fromLocalFile(currentDir.absoluteFilePath(files[i])),
                       arguments);
        emit progress(i, files.size());
    }

}
