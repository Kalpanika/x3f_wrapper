#ifndef CPROCESSINGTHREAD_H
#define CPROCESSINGTHREAD_H

#include <QThread>
#include <QStringList>
#include <QDir>
#include <QUrl>

QT_BEGIN_NAMESPACE
class QSettings;
class QStringList;
class QDir;
class QString;
QT_END_NAMESPACE


class CProcessingThread : public QThread
{

    Q_OBJECT
public:
    CProcessingThread(const QStringList& inFiles,
                      const QDir& inCurrentDir);

    void stopNow();

protected:
    void run();


signals:
    void progress(int currIndex, int totalNumber);
    void error_message(QString errorTitle, QString errorBody);
    void finishedProcessing();
    void canceledProcessing();

private:
    QSettings *settings;
    QStringList files;
    QDir currentDir;

    QStringList buildArgList();
    void convertX3FFile(const QUrl& fileName, const QStringList& inArgs);
    bool runX3FConversion(const QUrl& fileName, const QStringList& inArgs);
    bool runExifTool(QString exiftool, const QUrl& fileName, const QString &ff_corr);
    QString runExifTool_GetFFParam(const QString& exiftool, const QUrl& fileName, const bool& verbose);

    bool mRunning;
};

#endif // CPROCESSINGTHREAD_H
