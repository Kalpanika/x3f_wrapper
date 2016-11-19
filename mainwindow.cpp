#include <QtWidgets>
#include <QFileInfo>

#include "mainwindow.h"
#include "cpreferencespane.h"
#include "stringconstants.h"
#include "cprocessingthread.h"

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
    mainLayout->addWidget(filesConvertLabel, 2, 1);
    mainLayout->addWidget(convertAllButton, 2, 2);
    setLayout(mainLayout);

    setWindowTitle(tr("Convert X3F Files"));
    resize(700, 350);

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
    mPreferencesPane->exec(); // there are crashes if I use modal show, so I keep the dialog around instead
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

bool MainWindow::checkSettings(){
    // use this function to check to see if the settings are properly configured
    if (settings->value(StringConstants::x3fLocation).toString().length() < 1){
        QMessageBox::critical(NULL, "Please set the location of the x3f extractor executable",
                              "This program needs the x3f_extract program to be installed.  You can get it here: <a href=\"https://github.com/Kalpanika/x3f/releases\">https://github.com/Kalpanika/x3f/releases</a>");
        return false;
    }
    if (settings->value(StringConstants::exifToolsLocation).toString().length() < 1){
        QMessageBox::critical(NULL, "Please set the location of the exiftools executable",
                              "This program needs the exif tools program to be installed.  You can get it here: <a href=\"http://www.sno.phy.queensu.ca/~phil/exiftool/\">http://www.sno.phy.queensu.ca/~phil/exiftool/</a>");
        return false;
    }
    return true;
}

void MainWindow::convertFile(int row, int /* column */)
{
    if (!checkSettings()){
        return;
    }
    changeUI(false);
    QTableWidgetItem *item = filesTable->item(row, 0);

    QStringList files;
    files << item->text();
    completeFileList = files;
    convertAllFiles();
}

void MainWindow::updateProgress(int currIndex, int totalNumber){
    //this signal is received, but finishedProcessing is not...
    filesConvertLabel->setText(QString::number(currIndex + 1) +
                               " file(s) converted of " +
                               QString::number(totalNumber));
    if (currIndex + 1 == totalNumber){
        filesConvertLabel->setText(QString::number(currIndex + 1) + tr(" file(s) processed") +
                                 (" (Double click on a file to convert it)"));
        changeUI(true);
    }
    find();
}

void MainWindow::finishedProcessing(){
    filesConvertLabel->setText(tr("File(s) processed") +
                             (" (Double click on a file to convert it)"));
    changeUI(true);
    find();
}

void MainWindow::error_message(QString errorTitle, QString errorBody){
    QMessageBox::critical(this, errorTitle, errorBody);
}

void MainWindow::convertAllFiles()
{
    if (!checkSettings()){
        return;
    }

    changeUI(false);
    filesConvertLabel->setText("Processing...");
    QDir actualDirectory = QDir(currentDir);
    mProcessingThread = new CProcessingThread(completeFileList, actualDirectory);

    connect (mProcessingThread, SIGNAL(progress(int, int)), this, SLOT(updateProgress(int, int)));
    connect (mProcessingThread, SIGNAL(error_message(QString, QString)), this, SLOT(error_message(QString, QString)));
    connect (mProcessingThread, SIGNAL(finishedProcessing), this, SLOT(finishedProcessing));

    mProcessingThread->start();
}
