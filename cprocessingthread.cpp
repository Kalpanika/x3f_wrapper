#include "cprocessingthread.h"
#include <QSettings>
#include "settingsconstants.h"
#include <QProcess>


// see https://forum.qt.io/topic/60265/separate-qthread-to-update-qdialog-s-ui/3

CProcessingThread::CProcessingThread(const QStringList& inFiles,
                                     const QDir& inCurrentDir)
{
    settings = new QSettings;
    files = inFiles;
    currentDir = inCurrentDir;
    mRunning = true;
}

QStringList CProcessingThread::buildArgList(){
    QStringList output;
    if (!settings->value(SettingsConstants::denoise).toBool()){
        output << "-no-denoise";
    }
    if (settings->value(SettingsConstants::compress).toBool()){
        output << "-compress";
    }
    if (settings->value(SettingsConstants::ocl).toBool()){
        output << "-ocl";
    }
    // for the color, etc-- assume 0 is the default
    int val = settings->value(SettingsConstants::outputColor).toInt();
    if (val > 0){
        output << "-color";
        output << SettingsConstants::colorOptions[val];
    }
    val = settings->value(SettingsConstants::outputFormat).toInt();
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
    val = settings->value(SettingsConstants::outputWB).toInt();
    if (val > 0){
        output << "-wb"; // We sure this is the right way to set up the WB call?
        output << SettingsConstants::wbOptions[val];
    }
    return output;
}

bool CProcessingThread::runX3FConversion(const QUrl& fileName, const QStringList& inArgs){
    int format = settings->value(SettingsConstants::outputFormat).toInt();
    QString endingString = ".dng";
    switch (format){
    case 1:
        endingString = ".jpg";
        break;
    case 2:
        endingString = ".tif";
        break;
    }

    if (QFile::exists(fileName.toLocalFile() + endingString)){
        return true;
    }
    QString tmpFileName = fileName.toLocalFile() + ".dng.tmp";
    if (QFile::exists(tmpFileName)){
        bool removalSuccessful = QFile::remove(tmpFileName);
        if (!removalSuccessful){
            emit error_message("A temp file exists and needs to be deleted.",
                               "A file named " + tmpFileName + " exists and needs to be deleted before processing can continue.");
            return false;
        }
    }

    QString converter = settings->value(SettingsConstants::x3fLocation).toString();

    QStringList arguments = inArgs;
    arguments << fileName.toLocalFile();
    int exitCode = QProcess::execute(converter, arguments);
    if (exitCode == -2){
        emit error_message("Is the x3f extractor properly set?",
                           "Error code: " + QString::number(exitCode) +
                           " was returned, suggesting that the x3f executable provided in the preferences is not runnable.  Argument list: [" + arguments.join(", ") + "]");

        return false;
    }
    if (exitCode != 0){
        emit error_message("Something went wrong.",
                           "Something happened while processing the image.  Error code: " + QString::number(exitCode) +
                           " argument list: [" + arguments.join(", ") + "]");

        return false;
    }
    return true;
}

bool CProcessingThread::runExifTool(const QUrl& fileName){
    QString exiftools = settings->value(SettingsConstants::exifToolsLocation).toString();
    int format = settings->value(SettingsConstants::outputFormat).toInt();
    if (exiftools.length() < 1){
        return false;  // no specified exif tool location, so don't run it
    }
    QFileInfo info(exiftools);
    if (!info.isExecutable()){
        return false;  // not an executable
    }
    QStringList exiftoolsargs;
    exiftoolsargs << "-overwrite_original";
    exiftoolsargs << "-tagsFromFile";
    exiftoolsargs << fileName.toLocalFile();
    exiftoolsargs << "-all:all";
    int exitCode = 0;
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
    if (exitCode == -2){
        emit error_message("Is the exif extractor properly set?",
                           "Error code: " + QString::number(exitCode) +
                           " was returned, suggesting that the exif executable provided in the preferences is not runnable.  Argument list: [" + exiftoolsargs.join(", ") + "]");
        return false;
    }
    if (exitCode != 0){
        emit error_message("Something went wrong.",
                           "Something happened while transferring exif information.  Error code: " + QString::number(exitCode) +
                           " argument list: [" + exiftoolsargs.join(", ") + "]");
        return false;
    }
    return true;
}

void CProcessingThread::convertX3FFile(const QUrl& fileName, const QStringList& inArgs){
    if (runX3FConversion(fileName, inArgs) && mRunning){
        runExifTool(fileName);
    }
}

void CProcessingThread::stopNow(){
    mRunning = false;
}

void CProcessingThread::run()
{
    QStringList arguments = buildArgList();
    try{
        for (int i = 0; i < files.size(); ++i) {
            if (mRunning){
                convertX3FFile(QUrl::fromLocalFile(currentDir.absoluteFilePath(files[i])),
                               arguments);
                emit progress(i, files.size());
            }
        }
    } catch(...){
        //something broke, but for now, do nothing.
    }
    if (mRunning){
        emit finishedProcessing();
    } else {
        emit canceledProcessing();
    }
    //sleep(1);  //to make sure the final signal is emitted before the thread is killed
}
