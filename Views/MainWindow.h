#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include "Models/SourceBaseStationsTableModel.h"
#include "Database/Excel/XlsBaseStationsSource.h"
#include "Database/Excel/XlsxBaseStationsSource.h"
#include "Database/BaseStationsSourceTemplate.h"
#include "Converting/AbstractBaseStationsConverter.h"
#include "Database/SqliteDbStorage.h"
#include <QList>
#include <QDebug>
#include <QComboBox>
#include <QLineEdit>
#include <QGroupBox>
#include <QDir>
#include <QFile>
#include <QPushButton>
#include <memory>
#include <QProgressBar>

class WaitingSpinnerWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    bool mLoaded = false;
    QString templatesDir;

    void initLayout();
    void loadFile(const QString &fileName);
    void loadTemplates();
    void showTemplate(const std::shared_ptr<BaseStationsSourceTemplate> &t);
    void showDefaultTemplate();
    void setUiElementsEnabled(bool enabled);
    void setupConverter();
    void selectLastUsedTemplate();
    void updateDb(const QString& dbFileName);
    bool checkDatabase();
    bool initDatabase();

    QPushButton* showErrorsBth = nullptr;
    QProgressBar* progressBar = nullptr;
    QLineEdit* fileNameEdit = nullptr;
    QLineEdit* mDatabaseNameEdit = nullptr;
    QComboBox* pagesComboBox = nullptr;
    QComboBox* templatesComboBox = nullptr;
    QGroupBox* templateGroupBox = nullptr;
    QPushButton* convertBtn = nullptr;
    QPushButton* chooseFileBtn = nullptr;
    QPushButton* mChooseDatabaseBtn = nullptr;
    QPushButton* mDeleteDatabaseBtn = nullptr;
    QPushButton* templatesActionsBtn = nullptr;
    QTableView* table = nullptr;
    SourceBaseStationsTableModel* tableModel = nullptr;
    QThread* workerThread = nullptr;

    AbstractDbStorage* dbStorage = nullptr;
    AbstractBaseStationsConverter* converter = nullptr;
    ExcelBaseStationsSource* bsSource = nullptr;
    std::shared_ptr<BaseStationsSourceTemplate> currentSourceTemplate;
    QList<std::shared_ptr<BaseStationsSourceTemplate>> sourceTemplates;
    WaitingSpinnerWidget* mWaitingSpinner = nullptr;

private slots:
    void onConvertBtnPressed();
    void onChooseFileBtnPressed();
    void onChooseDatabaseBtnPressed();
    void onDeleteDatabaseBtnPressed();
    void onShowErrorsBtnPressed();
    void onTemplateIndexChanged(int index);

    void onSaveTemplate();
    void onSaveAsTemplate();
    void onDeleteTemplate();

    void onPageChanged(const QString& page);

    void onBsSourceError(int row, const QString& error);

    void onConverterStarted();
    void onConverterFinished(int added, int updated, int errors);
    void onConverterProgress(int progress);
    void onConverterError(int row, const QString& error);

protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // MAINWINDOW_H
