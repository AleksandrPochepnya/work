#ifndef XLSBASESTATIONSSOURCE_H
#define XLSBASESTATIONSSOURCE_H

#include "ExcelBaseStationsSource.h"
#include "BasicExcel/BasicExcel.hpp"

class XlsBaseStationsSource: public ExcelBaseStationsSource
{
public:
    explicit XlsBaseStationsSource(const QString& fileName, QObject *parent = nullptr);
    ~XlsBaseStationsSource() override;

    QStringList pages() override;

protected:
    QVariant read(int row, int column) const override;
    void selectPage(const QString pageName) override;

private:
     QStringList pagesList;

     YExcel::BasicExcel* xls = nullptr;
     YExcel::BasicExcelWorksheet* sheet = nullptr;
};

#endif // XLSBASESTATIONSSOURCE_H
