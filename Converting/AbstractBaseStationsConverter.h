#ifndef ABSTRACTBASESTATIONSCONVERTER_H
#define ABSTRACTBASESTATIONSCONVERTER_H

#include <QObject>
#include <QSet>
#include "Database/AbstractBaseStationsSource.h"
#include "Database/AbstractDbStorage.h"

class AbstractBaseStationsConverter : public QObject
{
    Q_OBJECT
public:
    explicit AbstractBaseStationsConverter(AbstractBaseStationsSource* bsSource,
                                           AbstractDbStorage* dbStorage,
                                           QObject *parent = nullptr);

    virtual ~AbstractBaseStationsConverter() = default;

    void convert(const QString& bsPage = "");
    void cancel();
    QStringList invalidRowsData() const;

    bool isRunning() const;

protected:
    AbstractBaseStationsSource* bsSource;
    AbstractDbStorage* dbStorage;

private:    
    bool isCanceled = false;
    bool mIsRunning = false;

    QSet<int> invalidRowsList;
    QStringList invalidRowsDataList;
    QString currentBsPage;

    void addInvalidRow(int row, const QString& err, bool flag = false);

signals:
    void started();
    void finished(int added, int updated, int errors);
    void error(int row, const QString& error);
    void progress(int progress);
};

#endif // ABSTRACTBASESTATIONSCONVERTER_H
