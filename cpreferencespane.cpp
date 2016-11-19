#include <QtWidgets>
#include "cpreferencespane.h"
#include "stringconstants.h"

CPreferencesPane::CPreferencesPane(QDialog *parent) : QDialog(parent)
{
    formatComboBox = new QComboBox;
    int i;
    for (i = 0; i < StringConstants::formatOptions.size(); i++){
        formatComboBox->addItem(StringConstants::formatOptions[i]);
    }
    formatComboBox->setEditable(false);
    formatLabel = new QLabel("Output Format:");

    extractLocationLabel = new QLabel("X3F_Extract Program Location:");
    exiftoolsLocationLabel = new QLabel("EXIF Tools Program Location:");

    colorComboBox = new QComboBox;
    colorLabel = new QLabel("Color Mode:");
    for (i = 0; i < StringConstants::colorOptions.size(); i++){
        formatComboBox->addItem(StringConstants::colorOptions[i]);
    };
    colorComboBox->setEditable(false);

    denoise = new QCheckBox("Denoise?  (usually yes)");

    compress = new QCheckBox("Compress output? (usually yes, only used on DNG and TIFF output)");

    ocl = new QCheckBox("Use OpenCL? (faster, less stable; use at your own risk)");

    whiteBalanceComboBox = new QComboBox;
    for (i = 0; i < StringConstants::wbOptions.size(); i++){
        formatComboBox->addItem(StringConstants::wbOptions[i]);
    };
    whiteBalanceLabel = new QLabel("White Balance:");

    saveButton = new QPushButton(tr("&Save"), this);
    connect(saveButton, &QAbstractButton::clicked,
            this, &CPreferencesPane::savePreferences);
    resetButton = new QPushButton(tr("&Reset"), this);
    connect(resetButton, &QAbstractButton::clicked,
            this, &CPreferencesPane::resetPreferences);

    extractLocation = new QLineEdit;
    exiftoolsLocation = new QLineEdit;
    browseX3FButton = new QPushButton(tr("&Browse"), this);
    connect(browseX3FButton, &QAbstractButton::clicked,
            this, &CPreferencesPane::browseX3F);
    browseEXIFButton = new QPushButton(tr("&Browse"), this);
    connect(browseEXIFButton, &QAbstractButton::clicked,
            this, &CPreferencesPane::browseEXIF);

    settings = new QSettings();
    loadPreferences();

    QGridLayout *preferencesLayout = new QGridLayout;
    int row = 0;
    preferencesLayout->addWidget(formatLabel, row, 0);
    preferencesLayout->addWidget(formatComboBox, row++, 1);
    preferencesLayout->addWidget(denoise, row++, 0, 1, 2);
    preferencesLayout->addWidget(compress, row++, 0, 1, 2);
    preferencesLayout->addWidget(ocl, row++, 0, 1, 2);
    preferencesLayout->addWidget(colorLabel, row, 0);
    preferencesLayout->addWidget(colorComboBox, row++, 1);
    preferencesLayout->addWidget(whiteBalanceLabel, row, 0);
    preferencesLayout->addWidget(whiteBalanceComboBox, row++, 1);
    preferencesLayout->addWidget(extractLocationLabel, row, 0);
    preferencesLayout->addWidget(extractLocation, row, 1);
    preferencesLayout->addWidget(browseX3FButton, row++, 2);
    preferencesLayout->addWidget(exiftoolsLocationLabel, row, 0);
    preferencesLayout->addWidget(exiftoolsLocation, row, 1);
    preferencesLayout->addWidget(browseEXIFButton, row++, 2);
    preferencesLayout->addWidget(saveButton, row, 0);
    preferencesLayout->addWidget(resetButton, row++, 2);
    setLayout(preferencesLayout);

    resize(600, 240);
    setWindowTitle(tr("Conversion Preferences"));
}


CPreferencesPane::~CPreferencesPane(){

}

void CPreferencesPane::loadPreferences(){
    denoise->setChecked(settings->value(StringConstants::denoise).toBool());
    compress->setChecked(settings->value(StringConstants::compress).toBool());
    ocl->setChecked(settings->value(StringConstants::ocl).toBool());
    formatComboBox->setCurrentIndex(settings->value(StringConstants::outputFormat).toInt());
    colorComboBox->setCurrentIndex(settings->value(StringConstants::outputColor).toInt());
    whiteBalanceComboBox->setCurrentIndex(settings->value(StringConstants::outputWB).toInt());
    extractLocation->setText(settings->value(StringConstants::x3fLocation).toString());
    exiftoolsLocation->setText(settings->value(StringConstants::exifToolsLocation).toString());
}

void CPreferencesPane::savePreferences(){
    settings->setValue(StringConstants::denoise, denoise->isChecked());
    settings->setValue(StringConstants::compress, compress->isChecked());
    settings->setValue(StringConstants::ocl, ocl->isChecked());
    settings->setValue(StringConstants::outputFormat, formatComboBox->currentIndex());
    settings->setValue(StringConstants::outputColor, colorComboBox->currentIndex());
    settings->setValue(StringConstants::outputWB, whiteBalanceComboBox->currentIndex());
    settings->setValue(StringConstants::x3fLocation, extractLocation->text());
    settings->setValue(StringConstants::exifToolsLocation, exiftoolsLocation->text());
    settings->sync();
}

void CPreferencesPane::resetPreferences(){
    denoise->setChecked(true);
    compress->setChecked(true);
    ocl->setChecked(true);
    formatComboBox->setCurrentIndex(0);
    colorComboBox->setCurrentIndex(0);
    whiteBalanceComboBox->setCurrentIndex(0);
    extractLocation->setText("");
    exiftoolsLocation->setText("");
    savePreferences();
}

void CPreferencesPane::browseX3F(){
    QDir currentDir = QDir::currentPath();
    QString directory = QFileDialog::getOpenFileName(this,
                                                     tr("Find X3F Extract Program"),
                                                     currentDir.absolutePath());
    extractLocation->setText(directory);
    savePreferences();
}

void CPreferencesPane::browseEXIF(){
    QDir currentDir = QDir::currentPath();
    QString directory = QFileDialog::getOpenFileName(this,
                                                     tr("Find EXIF Tools Program"),
                                                     currentDir.absolutePath());
    exiftoolsLocation->setText(directory);
    savePreferences();
}
