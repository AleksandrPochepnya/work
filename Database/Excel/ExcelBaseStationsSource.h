#ifndef EXCELBASESTATIONSSOURCE_H
#define EXCELBASESTATIONSSOURCE_H

#include "../AbstractBaseStationsSource.h"
#include "xlsxdocument.h"
#include <memory>

class ExcelBaseStationsSource : public AbstractBaseStationsSource
{
    Q_OBJECT
public:
    explicit ExcelBaseStationsSource(const QString& fileName, QObject *parent = nullptr);
    virtual ~ExcelBaseStationsSource() override;

    static QStringList columnNamesList();

    QStringList columnNames() const override;

protected:
    QString fileName;
};

#endif // EXCELBASESTATIONSSOURCE_H
