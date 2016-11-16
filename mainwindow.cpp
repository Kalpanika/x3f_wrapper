#include <QtWidgets>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent)
{

    browseButton = new QPushButton(tr("&Browse..."), this);
    connect(browseButton, &QAbstractButton::clicked, this, &MainWindow::browse);
    convertAllButton = new QPushButton(tr("&Convert all"), this);
    connect(convertAllButton, &QAbstractButton::clicked, this, &MainWindow::convertAllFiles);

    directoryComboBox = createComboBox(QDir::currentPath());

    directoryLabel = new QLabel(tr("In directory:"));
    filesConvertLabel = new QLabel;

    createFilesTable();

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(directoryLabel, 2, 0);
    mainLayout->addWidget(directoryComboBox, 2, 1);
    mainLayout->addWidget(browseButton, 2, 2);
    mainLayout->addWidget(filesTable, 3, 0, 1, 3);
    mainLayout->addWidget(filesConvertLabel, 4, 0, 1, 2);
    mainLayout->addWidget(convertAllButton, 4, 2);
    setLayout(mainLayout);

    setWindowTitle(tr("Convert X3F Files"));
    resize(700, 300);

    currentDir = QDir::currentPath(); //should use a registry entry or some such
}

MainWindow::~MainWindow()
{
//    delete ui;
}

void convertX3FFile(const QUrl& fileName){
#ifdef Q_OS_WIN32
    QString executableName = "Windows";
#elif Q_OS_MAC
    QString executableName = "Mac";
#elif Q_OS_LINUX
    QString executableName = "Linux";
#else
    QString executableName = "Not Supported";
#endif

    QMessageBox::information(NULL, executableName, fileName.fileName());
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

void MainWindow::showFiles(const QStringList &files)
{
    for (int i = 0; i < files.size(); ++i) {
        QFile file(currentDir.absoluteFilePath(files[i]));
        qint64 size = QFileInfo(file).size();

        QTableWidgetItem *fileNameItem = new QTableWidgetItem(files[i]);
        fileNameItem->setFlags(fileNameItem->flags() ^ Qt::ItemIsEditable);
        QTableWidgetItem *sizeItem = new QTableWidgetItem(tr("%1 KB")
                                             .arg(int((size + 1023) / 1024)));
        sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        sizeItem->setFlags(sizeItem->flags() ^ Qt::ItemIsEditable);

        int row = filesTable->rowCount();
        filesTable->insertRow(row);
        filesTable->setItem(row, 0, fileNameItem);
        filesTable->setItem(row, 1, sizeItem);
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
    filesTable = new QTableWidget(0, 2);
    filesTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    QStringList labels;
    labels << tr("Filename") << tr("Size");
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
