#include <QtWidgets>
#include <QFileInfo>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cpreferencespane.h"
#include "stringconstants.h"

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent)
{

    browseButton = new QPushButton(tr("&Browse..."), this);
    connect(browseButton, &QAbstractButton::clicked, this, &MainWindow::browse);

    configureButton = new QPushButton(tr("Con&figure Settings"), this);
    connect(configureButton, &QAbstractButton::clicked,
            this, &MainWindow::configureSettings);
    configureButton->setEnabled(true);

    convertAllButton = new QPushButton(tr("&Convert unprocessed"), this);
    connect(convertAllButton, &QAbstractButton::clicked,
            this, &MainWindow::convertAllFiles);

    //get settings here to get the page
    settings = new QSettings;
    int dirLen = settings->value(StringConstants::lastDir).toString().length();
    if (dirLen){
        currentDir = settings->value(StringConstants::lastDir).toString();
    } else {
        currentDir = QDir::currentPath();
    }

    directoryLineEdit = new QLineEdit;
    directoryLineEdit->setText(currentDir);

    directoryLabel = new QLabel(tr("In directory:"));
    filesConvertLabel = new QLabel;

    createFilesTable();

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(directoryLabel, 0, 0);
    mainLayout->addWidget(directoryLineEdit, 0, 1);
    mainLayout->addWidget(browseButton, 0, 2);
    mainLayout->addWidget(filesTable, 1, 0, 1, 3);
    mainLayout->addWidget(configureButton, 2, 0);
    mainLayout->addWidget(filesConvertLabel, 2, 1, 1, 2);
    mainLayout->addWidget(convertAllButton, 3, 2);
    setLayout(mainLayout);

    setWindowTitle(tr("Convert X3F Files"));
    resize(700, 300);

    mPreferencesPane = new CPreferencesPane();
    find();
}

MainWindow::~MainWindow()
{
//    delete ui;
}

void MainWindow::changeUI(const bool& ui_toggle){
    browseButton->setEnabled(ui_toggle);
    convertAllButton->setEnabled(ui_toggle);
    filesTable->setEnabled(ui_toggle);
    directoryLineEdit->setEnabled(ui_toggle);
    configureButton->setEnabled(ui_toggle);
}

void MainWindow::convertX3FFile(const QUrl& fileName, const QStringList& inArgs){
    if (QFile::exists(fileName.toLocalFile() + ".dng")){
        return;
    }
    QString tmpFileName = fileName.toLocalFile() + ".dng.tmp";
    if (QFile::exists(tmpFileName)){
        bool removalSuccessful = QFile::remove(tmpFileName);
        if (!removalSuccessful){
            QMessageBox::critical(NULL, "A temp file exists and needs to be deleted.",
                                  "A file named " + tmpFileName + " exists and needs to be deleted before processing can continue.");
            return;
        }
    }

    QString converter = settings->value(StringConstants::x3fLocation).toString();

    QStringList arguments = inArgs;
    arguments << fileName.toLocalFile();
    changeUI(false);
    int exitCode = QProcess::execute(converter, arguments);
    if (exitCode != 0){
        QMessageBox::critical(NULL, "Something went wrong.",
                              "Something happened while processing the image.  Error code: " + QString::number(exitCode));
    }
    changeUI(true);
    showFiles(completeFileList);
}

QStringList MainWindow::buildArgList(){
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

void MainWindow::browse()
{
    QString directory = QFileDialog::getExistingDirectory(this,
                                                          tr("Find X3F Files"),
                                                          currentDir);

    directoryLineEdit->setText(directory);

    currentDir = directory;
    settings->setValue(StringConstants::lastDir, directory);
    settings->sync();
    find();
}

void MainWindow::configureSettings(){
    //changeUI(false);
    mPreferencesPane->exec();
    //changeUI(true);
}

void MainWindow::find()
{
    filesTable->setRowCount(0);
    QString path = directoryLineEdit->text();

    currentDir = path;
    QStringList files;
    QString fileName = "*.x3f";
    files = QDir(currentDir).entryList(QStringList(fileName),
                                 QDir::Files | QDir::NoSymLinks);
    showFiles(files);
    completeFileList = files;
}

bool fileExists(const QString &path) {
    QFileInfo check_file(path);
    // check if file exists and if yes: Is it really a file and no directory?
    return check_file.exists() && check_file.isFile();
}

void MainWindow::showFiles(const QStringList &files)
{
    filesTable->setRowCount(0);
    QDir actualDirectory = QDir(currentDir);
    for (int i = 0; i < files.size(); ++i) {
        QFile file(actualDirectory.absoluteFilePath(files[i]));
        qint64 size = QFileInfo(file).size();
        QString hasExtractedFile = "Processed!";
        if (!fileExists(actualDirectory.absoluteFilePath(files[i]) + ".dng")){
            hasExtractedFile = "Not yet";
        }

        QTableWidgetItem *fileNameItem = new QTableWidgetItem(files[i]);
        fileNameItem->setFlags(fileNameItem->flags() ^ Qt::ItemIsEditable);
        QTableWidgetItem *dngItem = new QTableWidgetItem(hasExtractedFile);
        dngItem->setFlags(dngItem->flags() ^ Qt::ItemIsEditable);

        QTableWidgetItem *sizeItem = new QTableWidgetItem(tr("%1 KB")
                                             .arg(int((size + 1023) / 1024)));
        sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        sizeItem->setFlags(sizeItem->flags() ^ Qt::ItemIsEditable);

        int row = filesTable->rowCount();
        filesTable->insertRow(row);
        filesTable->setItem(row, 0, fileNameItem);
        filesTable->setItem(row, 1, dngItem);
        filesTable->setItem(row, 2, sizeItem);
    }
    filesConvertLabel->setText(tr("%1 file(s) found").arg(files.size()) +
                             (" (Double click on a file to convert it)"));
    filesConvertLabel->setWordWrap(true);
}

void MainWindow::createFilesTable()
{
    filesTable = new QTableWidget(0, 3);
    filesTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    QStringList labels;
    labels << tr("Filename") << tr("Processed?") <<  tr("Size");
    filesTable->setHorizontalHeaderLabels(labels);
    filesTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    filesTable->verticalHeader()->hide();
    filesTable->setShowGrid(false);

    connect(filesTable, &QTableWidget::cellActivated,
            this, &MainWindow::convertFile);
}\

void MainWindow::convertFile(int row, int /* column */)
{
    if (settings->value(StringConstants::x3fLocation).toString().length() < 1){
        QMessageBox::critical(NULL, "Please set the location of the x3f extractor executable",
                              "This program needs the x3f_extract program to be installed.");
        return;
    }
    if (settings->value(StringConstants::exifToolsLocation).toString().length() < 1){
        QMessageBox::critical(NULL, "Please set the location of the exiftools executable",
                              "This program needs the exif tools program to be installed.");
        return;
    }

    changeUI(false);
    filesConvertLabel->setText("Processing a single image.");
    QTableWidgetItem *item = filesTable->item(row, 0);

    QDir actualDirectory = QDir(currentDir);
    QStringList arguments = buildArgList();
    convertX3FFile(QUrl::fromLocalFile(actualDirectory.absoluteFilePath(item->text())),
                   arguments);
    filesConvertLabel->setText(tr("1 file(s) processed") +
                             (" (Double click on a file to convert it)"));

    changeUI(true);
}

void MainWindow::convertAllFiles()
{
    if (settings->value(StringConstants::x3fLocation).toString().length() < 1){
        QMessageBox::critical(NULL, "Please set the location of the x3f extractor executable",
                              "This program needs the x3f_extract program to be installed.");
        return;
    }
    if (settings->value(StringConstants::exifToolsLocation).toString().length() < 1){
        QMessageBox::critical(NULL, "Please set the location of the exiftools executable",
                              "This program needs the exif tools program to be installed.");
        return;
    }

    changeUI(false);
    filesConvertLabel->setText("Processing multiple images.");
    QDir actualDirectory = QDir(currentDir);
    QStringList arguments = buildArgList();
    for (int i = 0; i < completeFileList.size(); ++i) {
        convertX3FFile(QUrl::fromLocalFile(actualDirectory.absoluteFilePath(completeFileList[i])),
                       arguments);
        filesConvertLabel->setText(tr("%1 of %2 file(s) converted.").arg(i, completeFileList.size()));
    }
    filesConvertLabel->setText(tr("%1 file(s) converted").arg(completeFileList.size()) +
                             (" (Double click on a file to convert it)"));
    changeUI(true);

}
