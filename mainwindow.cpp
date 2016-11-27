#include <QtWidgets>
#include <QFileInfo>

#include "mainwindow.h"
#include "cpreferencespane.h"
#include "settingsconstants.h"
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
    int dirLen = settings->value(SettingsConstants::lastDir).toString().length();
    if (dirLen){
        currentDir = settings->value(SettingsConstants::lastDir).toString();
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
    if (!SettingsConstants::checkSettings(true)){
        configureSettings();
    }
    mRunning = false;
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
    if (ui_toggle){
        convertAllButton->setText("&Convert unprocessed");
    }
}

void MainWindow::browse()
{
    QString directory = QFileDialog::getExistingDirectory(this,
                                                          tr("Find X3F Files"),
                                                          currentDir);

    directoryLineEdit->setText(directory);

    currentDir = directory;
    settings->setValue(SettingsConstants::lastDir, directory);
    settings->sync();
    find();
}

void MainWindow::configureSettings(){
    //changeUI(false);
    mPreferencesPane->exec(); // there are crashes if I use modal show, so I keep the dialog around instead
    find();
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

    for (int i = 0; i < files.size(); ++i) {
        QFile file(actualDirectory.absoluteFilePath(files[i]));
        qint64 size = QFileInfo(file).size();
        QString hasExtractedFile = "Processed!";
        if (!fileExists(actualDirectory.absoluteFilePath(files[i]) + endingString)){
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
}

void MainWindow::convertFile(int row, int /* column */)
{
    changeUI(false);
    QTableWidgetItem *item = filesTable->item(row, 0);

    QStringList files;
    files << item->text();
    completeFileList = files;
    convertAllFiles();
}

void MainWindow::updateProgress(int currIndex, int totalNumber){
    //this signal is received, but finishedProcessing is not...
    find();
    filesConvertLabel->setText(QString::number(currIndex + 1) +
                               " file(s) converted of " +
                               QString::number(totalNumber));
}

void MainWindow::finishedProcessing(){
    changeUI(true);
    find();
    filesConvertLabel->setText(tr("File(s) were processed") +
                             (" (Double click on a file to convert it)"));
    mRunning = false;
}

void MainWindow::canceledProcessing(){
    changeUI(true);
    find();
    filesConvertLabel->setText(tr("File processing was canceled.") +
                             (" (Double click on a file to convert it)"));
    mRunning = false;
}

void MainWindow::error_message(QString errorTitle, QString errorBody){
    QMessageBox::critical(this, errorTitle, errorBody);
}

void MainWindow::convertAllFiles()
{
    if (!SettingsConstants::checkSettings(false)){
        return;
    }
    if (mRunning){
        mProcessingThread->stopNow();
        convertAllButton->setText("Canceling...");
        convertAllButton->setEnabled(false);
    }
    else{
        mRunning = true;
        changeUI(false);
        convertAllButton->setText("Cancel");
        convertAllButton->setEnabled(true);
        filesConvertLabel->setText("Processing...");
        QDir actualDirectory = QDir(currentDir);
        mProcessingThread = new CProcessingThread(completeFileList, actualDirectory);

        connect (mProcessingThread, SIGNAL(progress(int, int)), this, SLOT(updateProgress(int, int)));
        connect (mProcessingThread, SIGNAL(error_message(QString, QString)), this, SLOT(error_message(QString, QString)));
        connect (mProcessingThread, SIGNAL(finishedProcessing()), this, SLOT(finishedProcessing()));
        connect (mProcessingThread, SIGNAL(canceledProcessing()), this, SLOT(canceledProcessing()));

        mProcessingThread->start();
    }
}
