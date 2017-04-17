#include <QtWidgets>
#include "cpreferencespane.h"
#include "settingsconstants.h"

CPreferencesPane::CPreferencesPane(QDialog *parent) : QDialog(parent)
{
    formatComboBox = new QComboBox;
    int i;
    for (i = 0; i < SettingsConstants::formatOptions.size(); i++){
        formatComboBox->addItem(SettingsConstants::formatOptions[i]);
    }
    formatComboBox->setEditable(false);
    formatLabel = new QLabel("Output Format:");

    extractLocationLabel = new QLabel("X3F_Extract Program Location:");
    exiftoolsLocationLabel = new QLabel("EXIF Tools Program Location:");
    dngOpcodeLocationLabel = new QLabel("DNG Opcode File Location:");

    colorComboBox = new QComboBox;
    colorLabel = new QLabel("Color Mode:");
    for (i = 0; i < SettingsConstants::colorOptions.size(); i++){
        colorComboBox->addItem(SettingsConstants::colorOptions[i]);
    };
    colorComboBox->setEditable(false);

    denoise = new QCheckBox("Denoise?  (usually yes)");
    compress = new QCheckBox("Compress output? (usually yes, only used on DNG and TIFF output)");
    ocl = new QCheckBox("Use OpenCL? (faster, less stable; use at your own risk)");

    whiteBalanceComboBox = new QComboBox;
    for (i = 0; i < SettingsConstants::wbOptions.size(); i++){
        whiteBalanceComboBox->addItem(SettingsConstants::wbOptions[i]);
    };
    whiteBalanceLabel = new QLabel("White Balance:");

    saveButton = new QPushButton(tr("&Save"), this);
    connect(saveButton, &QAbstractButton::clicked,
            this, &CPreferencesPane::savePreferences);
    resetButton = new QPushButton(tr("&Reset"), this);
    connect(resetButton, &QAbstractButton::clicked,
            this, &CPreferencesPane::resetPreferences);
    closeButton = new QPushButton(tr("&Close"), this);
    connect(closeButton, &QAbstractButton::clicked,
            this, &QDialog::close);

    extractLocation = new QLineEdit;
    exiftoolsLocation = new QLineEdit;
    dngOpcodeLocation = new QLineEdit;
    browseX3FButton = new QPushButton(tr("&Browse"), this);
    connect(browseX3FButton, &QAbstractButton::clicked,
            this, &CPreferencesPane::browseX3F);
    browseEXIFButton = new QPushButton(tr("&Browse"), this);
    connect(browseEXIFButton, &QAbstractButton::clicked,
            this, &CPreferencesPane::browseEXIF);

    flatfield = new QCheckBox("Correct Merrill color cast (only for DNG output)");

    browseDNGOpcodeButton = new QPushButton(tr("&Browse"), this);
    connect(browseDNGOpcodeButton, &QAbstractButton::clicked,
            this, &CPreferencesPane::browseDNGOpcode);

    // Enable/disable flat-fielding options depending on output format and flat-field checkbox
    // not very elegant, but I have not really understood how to use signals and slots ...
    connect(flatfield, &QCheckBox::stateChanged, this, &CPreferencesPane::flatfield_changed);
    connect(formatComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &CPreferencesPane::format_changed);

    versionLabel = new QLabel("v. 0.56.3 Beta"); // 1st version of Merrill flat-fielding
    // MAKE SURE TO UPDATE THIS WITH EVERY NEW VERSION!!!


    settings = new QSettings();
    loadPreferences();

    QGridLayout *preferencesLayout = new QGridLayout;
    int row = 0;
    preferencesLayout->addWidget(formatLabel, row, 0);
    preferencesLayout->addWidget(formatComboBox, row++, 1, 1, 2);
    preferencesLayout->addWidget(denoise, row++, 0, 1, 2);
    preferencesLayout->addWidget(compress, row++, 0, 1, 2);
    preferencesLayout->addWidget(ocl, row++, 0, 1, 2);
    preferencesLayout->addWidget(colorLabel, row, 0);
    preferencesLayout->addWidget(colorComboBox, row++, 1, 1, 2);
    preferencesLayout->addWidget(whiteBalanceLabel, row, 0);
    preferencesLayout->addWidget(whiteBalanceComboBox, row++, 1, 1, 2);
    preferencesLayout->addWidget(extractLocationLabel, row, 0);
    preferencesLayout->addWidget(extractLocation, row, 1, 1, 2);
    preferencesLayout->addWidget(browseX3FButton, row++, 3);
    preferencesLayout->addWidget(exiftoolsLocationLabel, row, 0);
    preferencesLayout->addWidget(exiftoolsLocation, row, 1, 1, 2);
    preferencesLayout->addWidget(browseEXIFButton, row++, 3);
    preferencesLayout->addWidget(flatfield, row++, 0, 1, 2);
    preferencesLayout->addWidget(dngOpcodeLocationLabel, row, 0);
    preferencesLayout->addWidget(dngOpcodeLocation, row, 1, 1, 2);
    preferencesLayout->addWidget(browseDNGOpcodeButton, row++, 3);
    preferencesLayout->addWidget(versionLabel, row, 0);
    preferencesLayout->addWidget(saveButton, row, 1);
    preferencesLayout->addWidget(resetButton, row, 2);
    preferencesLayout->addWidget(closeButton, row++, 3);
    setLayout(preferencesLayout);

    resize(600, 240);
    setWindowTitle(tr("Conversion Preferences"));
}


CPreferencesPane::~CPreferencesPane(){

}

void CPreferencesPane::loadPreferences(){
    denoise->setChecked(settings->value(SettingsConstants::denoise).toBool());
    compress->setChecked(settings->value(SettingsConstants::compress).toBool());
    ocl->setChecked(settings->value(SettingsConstants::ocl).toBool());
    flatfield->setChecked(settings->value(SettingsConstants::flatfield).toBool());
    formatComboBox->setCurrentIndex(settings->value(SettingsConstants::outputFormat).toInt());
    colorComboBox->setCurrentIndex(settings->value(SettingsConstants::outputColor).toInt());
    whiteBalanceComboBox->setCurrentIndex(settings->value(SettingsConstants::outputWB).toInt());
    extractLocation->setText(settings->value(SettingsConstants::x3fLocation).toString());
    exiftoolsLocation->setText(settings->value(SettingsConstants::exifToolsLocation).toString());
    dngOpcodeLocation->setText(settings->value(SettingsConstants::dngOpcodeLocation).toString());
}

void CPreferencesPane::savePreferences(){
    settings->setValue(SettingsConstants::denoise, denoise->isChecked());
    settings->setValue(SettingsConstants::compress, compress->isChecked());
    settings->setValue(SettingsConstants::ocl, ocl->isChecked());
    settings->setValue(SettingsConstants::flatfield, flatfield->isChecked());
    settings->setValue(SettingsConstants::outputFormat, formatComboBox->currentIndex());
    settings->setValue(SettingsConstants::outputColor, colorComboBox->currentIndex());
    settings->setValue(SettingsConstants::outputWB, whiteBalanceComboBox->currentIndex());
    settings->setValue(SettingsConstants::x3fLocation, extractLocation->text());
    settings->setValue(SettingsConstants::exifToolsLocation, exiftoolsLocation->text());
    settings->setValue(SettingsConstants::dngOpcodeLocation, dngOpcodeLocation->text());
    settings->sync();
    if (!SettingsConstants::checkSettings(true)){
        return;
    }
}

void CPreferencesPane::resetPreferences(){
    denoise->setChecked(SettingsConstants::denoiseDefault);
    compress->setChecked(SettingsConstants::compressDefault);
    ocl->setChecked(SettingsConstants::oclDefault);
    flatfield->setChecked(SettingsConstants::flatfieldDefault);
    formatComboBox->setCurrentIndex(SettingsConstants::outputFormatDefault);
    colorComboBox->setCurrentIndex(SettingsConstants::outputColorDefault);
    whiteBalanceComboBox->setCurrentIndex(SettingsConstants::outputWBDefault);
    extractLocation->setText(SettingsConstants::x3fLocationDefault);
    exiftoolsLocation->setText(SettingsConstants::exifToolsLocationDefault);
    dngOpcodeLocation->setText(SettingsConstants::dngOpcodeLocationDefault);
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

// Look for directory with flat-fielding opcodes for Merrill cameras,
// the files contained need to be named <Model>_<Lens>_FF_DNG_OpcodeList3_<Aperture>
// <Model> can be "DP1M", "DP2M", "DP3M", "SD1M", "SD1"
// <Lens> is omitted for DP1/2/3M, because those have fixed lenses anyway
// <Aperture> is the aperture value as string, like "5.6" or "11"
void CPreferencesPane::browseDNGOpcode(){
    QDir currentDir = QDir::currentPath();
    QString directory = QFileDialog::getExistingDirectory(this, tr("Find DNG Opcode-Files Folder"),
                                                 currentDir.absolutePath(),
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    dngOpcodeLocation->setText(directory);
    savePreferences();
}


// flat-fielding selections only enabled if flat-field checkbox is checked
void CPreferencesPane::flatfield_changed(){
    if( flatfield->isChecked()) {
        browseDNGOpcodeButton->setEnabled(true);
        dngOpcodeLocation->setEnabled(true);
        dngOpcodeLocationLabel->setEnabled(true);
    }
    else {
        browseDNGOpcodeButton->setDisabled(true);
        dngOpcodeLocation->setDisabled(true);
        dngOpcodeLocationLabel->setDisabled(true);

    }
}

// flat-fielding options only for DNG output
void CPreferencesPane::format_changed(){
    if( formatComboBox->currentIndex() == 0) {
        flatfield->setEnabled(true);
        CPreferencesPane::flatfield_changed();
    }
    else {
        flatfield->setDisabled(true);
        browseDNGOpcodeButton->setDisabled(true);
        dngOpcodeLocation->setDisabled(true);
        dngOpcodeLocationLabel->setDisabled(true);

    }
}

