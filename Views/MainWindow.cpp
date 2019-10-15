#include "MainWindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include "Utils.h"
#include <QSpinBox>
#include "Views/BaseStationsSourceTemplateWidget.h"
#include "Views/ConvertingErrorsDialog.h"
#include "Views/Controls/WaitingSpinnerWidget.hpp"
#include <QHeaderView>
#include <QFont>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QThread>
#include "ApplicationSettings.h"
#include <QFileInfo>
#include <QProcess>
#include <QStatusBar>
#include <QDir>
#include "Build/version.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setMinimumSize(900, 600);
    setWindowTitle(QString("%1 %2").arg(APPNAME).arg(APPVERSION));

    templatesDir = QDir::currentPath() + QDir::separator() + "Templates";
    initLayout();
    loadTemplates();
    if(templatesComboBox->count() == 0)    
    {
        showDefaultTemplate();
    }
    else
    {
        selectLastUsedTemplate();
    }

    bool success = initDatabase();
    setUiElementsEnabled(success);

    if(!ApplicationSettings::instance().lastUsedBsFile.isEmpty())
    {
        loadFile(ApplicationSettings::instance().lastUsedBsFile);
    }

    mLoaded = true;

    if(!currentSourceTemplate->pageName.isEmpty())
    {
        int index = -1;
        if((index = pagesComboBox->findText(currentSourceTemplate->pageName)) == -1)
        {
            pagesComboBox->setStyleSheet("QComboBox {"
                                         "background-color: rgb(240,110,120,150);"
                                         "border: 1px solid #76797c;}");
            pagesComboBox->setToolTip(tr("Page %1 not found").arg(currentSourceTemplate->pageName));
        }
        else
        {
            pagesComboBox->setCurrentIndex(index);
        }
    }
}

void MainWindow::initLayout()
{
    QLabel* l = new QLabel(tr("<small>File with base stations:</small>"), this);
    l->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    l->setMaximumHeight(10);

    mDatabaseNameEdit = new QLineEdit(this);
    mDatabaseNameEdit->setReadOnly(true);
    mDatabaseNameEdit->setStyleSheet("QLineEdit { height:20px }");
    mDatabaseNameEdit->setText(ApplicationSettings::instance().dbFileName);
    mDatabaseNameEdit->setToolTip(ApplicationSettings::instance().dbFileName);

    mChooseDatabaseBtn = new QPushButton(tr("Choose"), this);
    mChooseDatabaseBtn->setStyleSheet("QPushButton { height:20px; font-weight:bold }");
    connect(mChooseDatabaseBtn, &QPushButton::pressed, this, &MainWindow::onChooseDatabaseBtnPressed);

    mDeleteDatabaseBtn = new QPushButton(tr("Delete"), this);
    mDeleteDatabaseBtn->setStyleSheet("QPushButton { height:20px; font-weight:bold }");
    connect(mDeleteDatabaseBtn, &QPushButton::pressed, this, &MainWindow::onDeleteDatabaseBtnPressed);

    fileNameEdit = new QLineEdit(this);
    fileNameEdit->setReadOnly(true);
    fileNameEdit->setStyleSheet("QLineEdit { height:20px }");

    chooseFileBtn = new QPushButton(tr("Open"), this);
    chooseFileBtn->setStyleSheet("QPushButton { height:20px; font-weight:bold }");
    connect(chooseFileBtn, &QPushButton::pressed, this, &MainWindow::onChooseFileBtnPressed);

    QHBoxLayout* chooseDbPanel = new QHBoxLayout();
    chooseDbPanel->setSpacing(0);
    chooseDbPanel->setContentsMargins(0,0,0,0);
    chooseDbPanel->addWidget(mDatabaseNameEdit);
    chooseDbPanel->setSpacing(5);
    chooseDbPanel->addWidget(mChooseDatabaseBtn);
    chooseDbPanel->addWidget(mDeleteDatabaseBtn);

    QHBoxLayout* chooseFilePanel = new QHBoxLayout();
    chooseFilePanel->setSpacing(0);
    chooseFilePanel->setContentsMargins(0,0,0,0);
    chooseFilePanel->addWidget(fileNameEdit);
    chooseFilePanel->setSpacing(5);
    chooseFilePanel->addWidget(chooseFileBtn);

    QVBoxLayout* fileChooseLayout = new QVBoxLayout();
    fileChooseLayout->setSpacing(0);
    fileChooseLayout->setContentsMargins(0,0,0,0);
    fileChooseLayout->addWidget(new QLabel(tr("<small>Database:</small>")));
    fileChooseLayout->addLayout(chooseDbPanel);
    fileChooseLayout->addWidget(l);
    fileChooseLayout->addLayout(chooseFilePanel);
    fileChooseLayout->setSizeConstraint(QLayout::SetMinimumSize);

    pagesComboBox = new QComboBox(this);
    connect(pagesComboBox, &QComboBox::currentTextChanged, this, &MainWindow::onPageChanged);

    l = new QLabel(tr("Page"), this);
    l->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

    QHBoxLayout* choosePageLayout = new QHBoxLayout();
    choosePageLayout->setContentsMargins(0,0,0,0);
    choosePageLayout->addWidget(l);
    choosePageLayout->addWidget(pagesComboBox);

    l = new QLabel(tr("Templates"), this);
    l->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

    templatesComboBox = new QComboBox(this);
    connect(templatesComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MainWindow::onTemplateIndexChanged);

    templatesActionsBtn = new QPushButton(tr("..."), this);
    templatesActionsBtn->setFixedWidth(40);
    templatesActionsBtn->setFixedHeight(23);
    templatesActionsBtn->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

    QMenu* menu = new QMenu("menu", this);
    menu->installEventFilter(this);
    QAction* saveAction = new QAction(tr("Save"), this);
    connect(saveAction, &QAction::triggered, this, &MainWindow::onSaveTemplate);

    QAction* saveAsAction = new QAction(tr("Save as..."), this);
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::onSaveAsTemplate);

    QAction* deleteAction = new QAction(tr("Delete"), this);
    connect(deleteAction, &QAction::triggered, this, &MainWindow::onDeleteTemplate);

    menu->addAction(saveAction);
    menu->addAction(saveAsAction);
    menu->addAction(deleteAction);

    templatesActionsBtn->setMenu(menu);

    QHBoxLayout* chooseTemplateLayout = new QHBoxLayout();
    chooseTemplateLayout->addWidget(l);
    chooseTemplateLayout->addWidget(templatesComboBox);
    chooseTemplateLayout->addWidget(templatesActionsBtn);

    templateGroupBox = new QGroupBox(tr("Template settings"));

    convertBtn = new QPushButton(tr("Convert"), this);
    convertBtn->setStyleSheet("QPushButton { font-weight:bold }");
    convertBtn->setEnabled(false);
    connect(convertBtn, &QPushButton::pressed, this, &MainWindow::onConvertBtnPressed);

    QVBoxLayout* toolsPanelLayout = new QVBoxLayout();
    toolsPanelLayout->setContentsMargins(0,0,0,0);
    toolsPanelLayout->addLayout(fileChooseLayout);
    toolsPanelLayout->addLayout(choosePageLayout);
    toolsPanelLayout->addLayout(chooseTemplateLayout);
    toolsPanelLayout->addWidget(templateGroupBox);
    toolsPanelLayout->addWidget(convertBtn);

    table = new QTableView(this);
    table->setSortingEnabled(false);
    table->setShowGrid(true);
    
    QFont tableFont = font();
    tableFont.setPointSize(8);
    table->setFont(tableFont);

    table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    table->verticalHeader()->setDefaultSectionSize(22);

    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->horizontalHeader()->setStretchLastSection(false);
    //table->verticalHeader()->hide();

    auto toolsPanelLayoutWidget = new QWidget(this);
    toolsPanelLayoutWidget->setContentsMargins(0,0,0,0);
    toolsPanelLayoutWidget->setLayout(toolsPanelLayout);

    progressBar = new QProgressBar(this);
    progressBar->hide();

    mWaitingSpinner = new WaitingSpinnerWidget(this, false, false);
    mWaitingSpinner->setFixedSize(25, 25);
    mWaitingSpinner->hide();

    QGridLayout* layout = new QGridLayout();
    layout->addWidget(toolsPanelLayoutWidget, 0, 0, 2, 1);
    layout->addWidget(table, 0, 1, 1, 2);
    layout->addWidget(mWaitingSpinner, 1, 1);
    layout->addWidget(progressBar, 1, 2);
    layout->setColumnStretch(1, 2);

    auto centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);

    setCentralWidget(centralWidget);

    setStatusBar(new QStatusBar(this));

    showErrorsBth = new QPushButton(this);
    connect(showErrorsBth, &QPushButton::pressed, this, &MainWindow::onShowErrorsBtnPressed);
    showErrorsBth->setStyleSheet("background-color: lightgray; color: red; border: 1px; margin-left: 10px; margin-right: 10px; padding-left: 10px; padding-right: 10px");
    showErrorsBth->setText(tr("Show errors"));
    statusBar()->addPermanentWidget(showErrorsBth);

    showErrorsBth->hide();
    statusBar()->hide();
}

void MainWindow::loadFile(const QString& fileName)
{
    qDebug() << "Load file" << fileName;

    if(!QFile(fileName).exists())
    {
        QMessageBox::warning(this, tr("Loading error"), QString(tr("File %1 does not exists")).arg(fileName));
        return;
    }

    QString ext = QFileInfo(fileName).suffix();
    if(ext == "xls")
    {
        QMessageBox::warning(this, tr("Warning"), tr("Cannot open file %1. Convert this file to format *.xlsx").arg(fileName));
        return;
    }

    if(bsSource != nullptr)
    {
        delete bsSource;
    }

    if(ext == "xls")    
    {
        bsSource = new XlsBaseStationsSource(fileName);
    }
    else if (ext == "xlsx")
    {
        bsSource = new XlsxBaseStationsSource(fileName);
    }
    else
    {
        qDebug() << "Unknown file extension" << ext << "!!!!!!!!!!!!!!";
        return;
    }

    connect(bsSource, &ExcelBaseStationsSource::error, this, &MainWindow::onBsSourceError);
    bsSource->setTemplate(currentSourceTemplate);

    setupConverter();

    if(tableModel != nullptr)
    {
        delete tableModel;
    }

    auto& settings = ApplicationSettings::instance();
    tableModel = new SourceBaseStationsTableModel(bsSource, settings.previewRowsCount, settings.previewColumnsCount, this);
    table->setModel(tableModel);

    pagesComboBox->clear();
    QStringList pages = bsSource->pages();

    for(const auto& page: pages)
    {
        pagesComboBox->addItem(page);
    }

    fileNameEdit->setText(fileName);
    fileNameEdit->setToolTip(fileName);
    convertBtn->setEnabled(true);

    settings.lastUsedBsFile = fileName;
}

void MainWindow::loadTemplates()
{
    qDebug() << "Load templates";

    QDir dir(templatesDir);
    if(!dir.exists())
    {
        qDebug() << "Templates directory does not exists";
        return;
    }

    sourceTemplates.clear();
    templatesComboBox->clear();

    QStringList filters;
    filters << "*.json";
    QStringList files = dir.entryList(filters);

    for(const auto& file: files)
    {
        auto t = std::shared_ptr<BaseStationsSourceTemplate>(new BaseStationsSourceTemplate(dir.filePath(file), this));
        sourceTemplates.append(t);
        templatesComboBox->addItem(file.left(file.size() - 5));
    }
}

void MainWindow::showTemplate(const std::shared_ptr<BaseStationsSourceTemplate> &t)
{
    qDebug() << "Show template";
    auto layout = templateGroupBox->layout();
    if(layout != nullptr)
    {
        Utils::clearLayout(layout);
        delete layout;
    }

    QGridLayout* grid = new QGridLayout();
    grid->setContentsMargins(0,0,0,0);

    auto widget = new BaseStationsSourceTemplateWidget(t, this);
    grid->addWidget(widget, 0, 0);
    templateGroupBox->setLayout(grid);
}

void MainWindow::showDefaultTemplate()
{
    currentSourceTemplate = std::shared_ptr<BaseStationsSourceTemplate>(new BaseStationsSourceTemplate(this));
    showTemplate(currentSourceTemplate);
}

void MainWindow::setUiElementsEnabled(bool enabled)
{
    chooseFileBtn->setEnabled(enabled);
    pagesComboBox->setEnabled(enabled);
    templateGroupBox->setEnabled(enabled);
    templatesComboBox->setEnabled(enabled);
    templatesActionsBtn->setEnabled(enabled);
}

void MainWindow::setupConverter()
{
    if(converter != nullptr)
    {
        delete converter;
    }

    converter = new AbstractBaseStationsConverter(bsSource, dbStorage);
    connect(converter, &AbstractBaseStationsConverter::started, this, &MainWindow::onConverterStarted);
    connect(converter, &AbstractBaseStationsConverter::finished, this, &MainWindow::onConverterFinished);
    connect(converter, &AbstractBaseStationsConverter::error, this, &MainWindow::onConverterError);
    connect(converter, &AbstractBaseStationsConverter::progress, this, &MainWindow::onConverterProgress);
}

void MainWindow::selectLastUsedTemplate()
{
    QString lastUsedTemplate = ApplicationSettings::instance().lastUsedTemplate;
    auto iter = std::find_if(sourceTemplates.begin(), sourceTemplates.end(),
                             [&lastUsedTemplate](const std::shared_ptr<BaseStationsSourceTemplate>& other){
        return QFileInfo(other->fileName).baseName() == lastUsedTemplate;
    });

    if(iter != sourceTemplates.end())
    {
        currentSourceTemplate = *iter;
        int index = std::distance(sourceTemplates.begin(), iter);
        templatesComboBox->setCurrentIndex(index);
    }
}

void MainWindow::updateDb(const QString &dbFileName)
{
    qDebug() << "Update database" << dbFileName;

    QFile utilFile(QFileInfo(QDir("dbupdate").filePath("GeoMapsDbUpdate.exe")).absoluteFilePath());
    if(!utilFile.exists())
    {
        QString msg = QString(tr("Utility %1 does not exists").arg(utilFile.fileName()));
        qDebug() << msg;
        QMessageBox::warning(this, tr("Error"), msg);
        return;
    }

    QProcess* process = new QProcess(this);

    function<void()> processOutput = [=]() {
         qDebug() << QString(process->readAllStandardOutput());
         qDebug() << QString(process->readAllStandardError());
    };

    connect(process, &QProcess::readyReadStandardOutput, this, processOutput);
    connect(process, &QProcess::readyReadStandardError, this, processOutput);

    process->setProcessChannelMode(QProcess::MergedChannels);
    process->setWorkingDirectory("dbupdate");
    process->start(utilFile.fileName(), QStringList() << dbFileName);
    process->waitForFinished();

    if(process->exitCode() != 0)
    {
        QMessageBox::warning(this, tr("Error"), process->errorString());
        return;
    }

    qDebug() << "Process" << process->program() << process->arguments().join(' ') << "successfully exited";
}

bool MainWindow::checkDatabase()
{
    qDebug() << "Check database";
    QFile dbFile(ApplicationSettings::instance().dbFileName);
    QFile emptyDbFile(ApplicationSettings::instance().emptyDbFileName);

    if(dbFile.exists())
    {
        if(dbFile.size() != 0)
        {
            qDebug() << "Database already exists and not empty file";
            return true;
        }

        qDebug() << "Database is empty. Remove:" << ApplicationSettings::instance().dbFileName;
        if(!dbFile.remove())
        {
            qDebug() << "Cannot delete database file" << dbFile.errorString();
            return false;
        }
    }

    if(!emptyDbFile.exists())
    {
        qDebug() << "Empty database does not exists:" << ApplicationSettings::instance().emptyDbFileName;
        return false;
    }

    qDebug() << "Copy empty database as work database";
    return emptyDbFile.copy(ApplicationSettings::instance().dbFileName);
}

bool MainWindow::initDatabase()
{
    if(!checkDatabase())
    {
        QMessageBox::warning(this, tr("Error"), tr("Cannot init database %1").arg(ApplicationSettings::instance().dbFileName));
        return false;
    }

    auto dbFileName = ApplicationSettings::instance().dbFileName;

    if(dbStorage != nullptr)
    {
        delete dbStorage;
    }

    dbStorage = new SqliteDbStorage(dbFileName, this);
    if(!dbStorage->open())
    {
        QMessageBox::warning(this, tr("Error"), dbStorage->lastError());
        return false;
    }

    if(!dbStorage->init())
    {
        QMessageBox::warning(this, tr("Error"), dbStorage->lastError());
        return false;
    }

    return true;
}

void MainWindow::onConvertBtnPressed()
{
    if(converter->isRunning())
    {
        converter->cancel();
        return;
    }

    qDebug() << "Convert";

    if(workerThread != nullptr)
    {
        workerThread->terminate();
        workerThread->wait();
        delete workerThread;
    }

    setupConverter();

    workerThread = new QThread(this);
    converter->moveToThread(workerThread);
    connect(workerThread, &QThread::started, this, [=](){
        converter->convert(pagesComboBox->currentText());
    }, Qt::DirectConnection);

    workerThread->start();
    convertBtn->setText(tr("Cancel"));
}

void MainWindow::onChooseFileBtnPressed()
{
    qDebug() << "Choose file";

    QString startPath = "";
#ifdef QT_DEBUG
    startPath = QDir::currentPath();
#endif

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), startPath , tr("Excel files (*.xlsx *xls)"));
    if(fileName == "")
    {
        return;
    }

    loadFile(fileName);
}

void MainWindow::onChooseDatabaseBtnPressed()
{
    qDebug() << "Choose database";

    QString startPath = "";
#ifdef QT_DEBUG
    startPath = QDir::currentPath();
#endif

    QString fileName = QFileDialog::getOpenFileName(this, tr("Choose database"), startPath , tr("SQLite database (*.db)"));
    if(fileName == "")
    {
        return;
    }

    ApplicationSettings::instance().dbFileName = fileName;

    mDatabaseNameEdit->setText(fileName);
    mDatabaseNameEdit->setToolTip(fileName);


    bool success = initDatabase();

    if(success)
    {
        setupConverter();
    }

    setUiElementsEnabled(success);
}

void MainWindow::onDeleteDatabaseBtnPressed()
{
    qDebug() << "Delete database";

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(ApplicationSettings::instance().dbFileName);
    if (!db.open()) {
        qDebug() << "Cannot open database:" << db.lastError();
        QMessageBox::information(this, tr("Attention"), tr("Cannot open database"));
        db.close();
        return ;
    }

    QSqlQuery query;
    if (!query.exec("DELETE from CELLS;")) {
        qDebug() << "Unable to execute query - exiting";
        QMessageBox::information(this, tr("Attention"), tr("Deletion was not successful"));
    }
    else
       QMessageBox::information(this, tr("Attention"), tr("Deletion was successful"));
    db.close();
}

void MainWindow::onShowErrorsBtnPressed()
{
    ConvertingErrorsDialog* dialog = new ConvertingErrorsDialog(converter, this);
    dialog->setModal(true);
    dialog->exec();
    dialog->deleteLater();
}

void MainWindow::onTemplateIndexChanged(int index)
{
    if(index < 0 || index >= sourceTemplates.size())
    {
        return;
    }

    qDebug() << "Template index changed" << index;

    auto t = sourceTemplates.at(index);
    currentSourceTemplate = t;
    if(bsSource != nullptr)
    {
        bsSource->setTemplate(currentSourceTemplate);
    }
    showTemplate(currentSourceTemplate);

    if(mLoaded)
    {
        ApplicationSettings::instance().lastUsedTemplate = QFileInfo(currentSourceTemplate->fileName).baseName();
    }
}

void MainWindow::onSaveTemplate()
{
    if(currentSourceTemplate.get() == nullptr)
    {
       qDebug() << "Current template is null";
       return;
    }    
    
    currentSourceTemplate->pageName = pagesComboBox->currentText();
    currentSourceTemplate->save();
}

void MainWindow::onSaveAsTemplate()
{
   if(currentSourceTemplate.get() == nullptr)
    {
       qDebug() << "Current template is null";
       return;
    }

   if(!QDir(templatesDir).exists())
   {
       QDir().mkdir(templatesDir);
   }

    QString fileName = QFileDialog::getSaveFileName(this, tr("Open File"), templatesDir, tr("Json file (*.json)"));
    if(fileName == "")
    {
        return;
    }

    currentSourceTemplate->pageName = pagesComboBox->currentText();
    currentSourceTemplate->saveAs(fileName);
    loadTemplates();
    if(sourceTemplates.size() == 0)
    {
        return;
    }
    
    auto iter = std::find_if(sourceTemplates.begin(), sourceTemplates.end(),
                             [&fileName](const std::shared_ptr<BaseStationsSourceTemplate>& other){
        return other->fileName == fileName;
    });

    if(iter != sourceTemplates.end())
    {
        currentSourceTemplate = *iter;
        int index = std::distance(sourceTemplates.begin(), iter);
        templatesComboBox->setCurrentIndex(index);
    }
}

void MainWindow::onDeleteTemplate()
{
   if(currentSourceTemplate.get() == nullptr)
    {
       qDebug() << "Current template is null";
       return;
    }

    if(QMessageBox::question(this, tr("Delete template"), 
        QString(tr("Are you sure you want to delete template %1?")).arg(currentSourceTemplate->fileName))
            != QMessageBox::Yes)
    {
        return;
    }

    currentSourceTemplate->remove();
    loadTemplates();

    if(sourceTemplates.isEmpty())
    {
        showDefaultTemplate();
    }
}

void MainWindow::onPageChanged(const QString& page)
{
    qDebug() << "Page changed" << page;
    if(page.isEmpty())
    {
        return;
    }

    tableModel->update(page);
    pagesComboBox->setStyleSheet("");
    pagesComboBox->setToolTip("");
}

void MainWindow::onBsSourceError(int row, const QString &error)
{
    qDebug() << "Base stations source error: row" << row << error;
//    QMessageBox::warning(this, tr("Error"), error);
}

void MainWindow::onConverterStarted()
{
    qDebug() << "Converting started";
    progressBar->setValue(0);
    setUiElementsEnabled(false);
    mChooseDatabaseBtn->setEnabled(false);
    mDeleteDatabaseBtn->setEnabled(false);
    statusBar()->showMessage(tr("Converting..."));
    progressBar->show();
    mWaitingSpinner->show();
    mWaitingSpinner->start();
}

void MainWindow::onConverterFinished(int added, int updated, int errors)
{
    qDebug() << "Converting finished: added" << added << "updated" << updated << "errors" << errors;

    statusBar()->show();
    QString status = tr("Converting finished: added %1 base stations, updated %2 base stations, %3 errors").arg(added).arg(updated).arg(errors);
    convertBtn->setText(tr("Convert"));

    statusBar()->showMessage(status);

    if(errors > 0)
    {
        showErrorsBth->show();
    }

    if(ApplicationSettings::instance().updateDbAfterConverting)
    {
        updateDb(QFileInfo(ApplicationSettings::instance().dbFileName).absoluteFilePath());
    }

    setUiElementsEnabled(true);
    mChooseDatabaseBtn->setEnabled(true);
    mDeleteDatabaseBtn->setEnabled(true);
    progressBar->hide();
    mWaitingSpinner->hide();
    mWaitingSpinner->stop();
}

void MainWindow::onConverterProgress(int progress)
{
    progressBar->setValue(progress);
}

void MainWindow::onConverterError(int row, const QString &error)
{
    qDebug() << "Converting error: row" << row + currentSourceTemplate->firstRow << error;
    QMessageBox::warning(this, tr("Converting error"), error);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(workerThread != nullptr)
    {
        converter->cancel();
        workerThread->quit();
        workerThread->wait();
    }

    ApplicationSettings::instance().save();

    QMainWindow::closeEvent(event);
}
