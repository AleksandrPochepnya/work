#ifndef ABSTRACTBASESTATIONSSOURCE_H
#define ABSTRACTBASESTATIONSSOURCE_H

#include <QObject>
#include <QStringList>
#include <QList>
#include "Types/BaseStation.h"
#include <memory>
#include "BaseStationsSourceTemplate.h"
#include <QDebug>

class AbstractBaseStationsSource: public QObject
{
    Q_OBJECT
public:
    virtual QStringList pages() = 0;
    virtual QStringList columnNames() const = 0;

    virtual QString readRowFromPage(const QString& pageName, int row);
    virtual QList<BaseStation> readFromPage(const QString& pageName);
    virtual QList<QStringList> readAllFromPage(const QString& pageName, int rowsCount, int columnsCount);

    void setTemplate(const std::shared_ptr<BaseStationsSourceTemplate> &value);
    std::shared_ptr<BaseStationsSourceTemplate> GetsourceTemplate();

protected:
    AbstractBaseStationsSource(QObject* parent = nullptr);
    virtual ~AbstractBaseStationsSource() = default;

    virtual QVariant read(int row, int column) const = 0;
    virtual void selectPage(const QString pageName) = 0;

    std::shared_ptr<BaseStationsSourceTemplate> sourceTemplate;
    QRegExp mCoordinatesRegExp;
    QRegExp mNumberRegExp;

    void onError(BaseStation::Field field, int row) const;
    bool isRowEmpty(int row) const;
    double parseGpsCoordinate(BaseStation::Field field, QString text, int row);

signals:
    void error(int row, const QString& error) const;
};

#endif // ABSTRACTBASESTATIONSSOURCE_H
