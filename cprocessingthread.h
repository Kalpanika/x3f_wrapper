#ifndef CPROCESSINGTHREAD_H
#define CPROCESSINGTHREAD_H

#include <QThread>

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


protected:
    void run();


signals:
    void progress(int currIndex, int totalNumber);
    void error_message(QString errorTitle, QString errorBody);
    void finished();

private:
    QSettings settings;
    QStringList files;
    QDir currentDir;
};

#endif // CPROCESSINGTHREAD_H
