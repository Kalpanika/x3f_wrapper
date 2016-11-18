#include <QtWidgets>
#include <QFileInfo>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cpreferencespane.h"

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent)
{

    browseButton = new QPushButton(tr("&Browse..."), this);
    connect(browseButton, &QAbstractButton::clicked, this, &MainWindow::browse);
    convertAllButton = new QPushButton(tr("&Convert all"), this);
    connect(convertAllButton, &QAbstractButton::clicked,
            this, &MainWindow::convertAllFiles);

    configureButton = new QPushButton(tr("Con&figure Settings"), this);
    connect(configureButton, &QAbstractButton::clicked,
            this, &MainWindow::configureSettings);

    directoryComboBox = createComboBox(QDir::currentPath());

    directoryLabel = new QLabel(tr("In directory:"));
    filesConvertLabel = new QLabel;

    createFilesTable();

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(directoryLabel, 2, 0);
    mainLayout->addWidget(directoryComboBox, 2, 1);
    mainLayout->addWidget(browseButton, 2, 2);
    mainLayout->addWidget(filesTable, 3, 0, 1, 3);
    mainLayout->addWidget(filesConvertLabel, 4, 1, 1, 2);
    mainLayout->addWidget(convertAllButton, 4, 2);
    mainLayout->addWidget(configureButton, 4, 0);
    setLayout(mainLayout);

    setWindowTitle(tr("Convert X3F Files"));
    resize(700, 300);

    mPreferencesPane = new CPreferencesPane();

    currentDir = QDir::currentPath(); //should use a registry entry or some such
}

MainWindow::~MainWindow()
{
//    delete ui;
}

void MainWindow::changeUI(const bool& ui_toggle){
    browseButton->setEnabled(ui_toggle);
    convertAllButton->setEnabled(ui_toggle);
    filesTable->setEnabled(ui_toggle);
    directoryComboBox->setEnabled(ui_toggle);
    configureButton->setEnabled(ui_toggle);
}

void MainWindow::convertX3FFile(const QUrl& fileName){
#ifdef Q_OS_WIN32
    QString executableName = "d:/src/x3f_wrapper/x3f_extract.exe";
#elif Q_OS_MAC
    QString executableName = "Mac";
#elif Q_OS_LINUX
    QString executableName = "Linux";
#else
    QString executableName = "Not Supported";
#endif

    QStringList arguments;
    arguments << "-color";
    arguments << "ProPhotoRGB";
    arguments << fileName.toLocalFile();
    changeUI(false);
    int exitCode = QProcess::execute(executableName, arguments);
    if (exitCode != 0){
        QMessageBox::critical(NULL, "Something went wrong.",
                              "Something happened while processing the image.  Error code:" + QString::number(exitCode));
    }
    changeUI(true);
    showFiles(completeFileList);
}

void MainWindow::browse()
{
    QString directory = QFileDialog::getExistingDirectory(this,
                               tr("Find X3F Files"), currentDir.absolutePath());

    if (!directory.isEmpty()) {
        if (directoryComboBox->findText(directory) == -1)
            directoryComboBox->addItem(directory);
        directoryComboBox->setCurrentIndex(directoryComboBox->findText(directory));
    }

    find();
}

void MainWindow::configureSettings(){
    changeUI(false);
    mPreferencesPane->exec();
    changeUI(true);
}

static void updateComboBox(QComboBox *comboBox)
{
    if (comboBox->findText(comboBox->currentText()) == -1)
        comboBox->addItem(comboBox->currentText());
}

void MainWindow::find()
{
    filesTable->setRowCount(0);
    QString path = directoryComboBox->currentText();
    updateComboBox(directoryComboBox);

    currentDir = QDir(path);
    QStringList files;
    QString fileName = "*.x3f";
    files = currentDir.entryList(QStringList(fileName),
                                 QDir::Files | QDir::NoSymLinks);

    files = findFiles(files);
    showFiles(files);
    completeFileList = files;
    currentDir = QDir(path);
}

QStringList MainWindow::findFiles(const QStringList &files)
{
    QProgressDialog progressDialog(this);
    progressDialog.setCancelButtonText(tr("&Cancel"));
    progressDialog.setRange(0, files.size());
    progressDialog.setWindowTitle(tr("Convert X3F Files"));

    QStringList foundFiles;

    for (int i = 0; i < files.size(); ++i) {
        progressDialog.setValue(i);
        progressDialog.setLabelText(tr("Searching file number %1 of %2...")
                                    .arg(i).arg(files.size()));
        qApp->processEvents();

        if (progressDialog.wasCanceled())
            break;

        foundFiles << files[i];
    }
    return foundFiles;
}

bool fileExists(const QString &path) {
    QFileInfo check_file(path);
    // check if file exists and if yes: Is it really a file and no directory?
    return check_file.exists() && check_file.isFile();
}

void MainWindow::showFiles(const QStringList &files)
{
    filesTable->setRowCount(0);
    for (int i = 0; i < files.size(); ++i) {
        QFile file(currentDir.absoluteFilePath(files[i]));
        qint64 size = QFileInfo(file).size();
        QString hasExtractedFile = "Processed!";
        if (!fileExists(currentDir.absoluteFilePath(files[i]) + ".dng")){
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

QComboBox *MainWindow::createComboBox(const QString &text)
{
    QComboBox *comboBox = new QComboBox;
    comboBox->setEditable(true);
    comboBox->addItem(text);
    comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    return comboBox;
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
    QTableWidgetItem *item = filesTable->item(row, 0);

    convertX3FFile(QUrl::fromLocalFile(currentDir.absoluteFilePath(item->text())));
}

void MainWindow::convertAllFiles()
{
    for (int i = 0; i < completeFileList.size(); ++i) {
        convertX3FFile(QUrl::fromLocalFile(currentDir.absoluteFilePath(completeFileList[i])));
    }
}
