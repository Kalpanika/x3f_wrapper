#ifndef CPREFERENCESPANE_H
#define CPREFERENCESPANE_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QComboBox;
class QLabel;
class QPushButton;
class QCheckBox;
class QLineEdit;
QT_END_NAMESPACE

class CPreferencesPane : public QDialog
{
    Q_OBJECT
public:
    explicit CPreferencesPane(QDialog *parent = 0);
    ~CPreferencesPane();

signals:

private slots:
    void savePreferences();
    void resetPreferences();
    void browseX3F();
    void browseEXIF();

private:
    void loadPreferences();

    QLabel *colorLabel;
    QLabel *whiteBalanceLabel;
    QLabel *formatLabel;
    QCheckBox *denoise;
    QCheckBox *compress;
    QCheckBox *ocl;
    QComboBox *formatComboBox;
    QComboBox *colorComboBox;
    QComboBox *whiteBalanceComboBox;
    QPushButton *saveButton;
    QPushButton *resetButton;
    QLabel *extractLocationLabel;
    QLabel *exiftoolsLocationLabel;
    QLineEdit *extractLocation;
    QLineEdit *exiftoolsLocation;
    QPushButton *browseX3FButton;
    QPushButton *browseEXIFButton;
};

#endif // CPREFERENCESPANE_H
