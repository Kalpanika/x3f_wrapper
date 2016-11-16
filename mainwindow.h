#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QDir>

QT_BEGIN_NAMESPACE
class QComboBox;
class QLabel;
class QPushButton;
class QTableWidget;
class QTableWidgetItem;
QT_END_NAMESPACE

/*
 * This basic class borrows heavily from the findfiles example.
 * From that point, the code has been modified such that, once files
 * have been found and listed, when they are 'opened', processes
 * are spawned to convert each file via the x3f_tools executable.
 * Ultimately, these same files will then have their exif information
 * also ported over, probably via exiftools.
 */

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
    void browse();
    void find();
    void convertFile(int row, int column);
    void convertAllFiles();

private:

    QStringList findFiles(const QStringList &files);
    void showFiles(const QStringList &files);
    QComboBox *createComboBox(const QString &text = QString());
    void createFilesTable();

    QComboBox *directoryComboBox;
    QLabel *directoryLabel;
    QLabel *filesConvertLabel;
    QPushButton *browseButton;
    QPushButton *convertAllButton;
    QTableWidget *filesTable;

    QStringList completeFileList;

    QDir currentDir;
};

#endif // MAINWINDOW_H
