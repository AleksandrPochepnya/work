#ifndef XLSXBASESTATIONSSOURCE_H
#define XLSXBASESTATIONSSOURCE_H

#include "ExcelBaseStationsSource.h"

class XlsxBaseStationsSource: public ExcelBaseStationsSource
{
public:
    explicit XlsxBaseStationsSource(const QString& fileName, QObject *parent = nullptr);
    ~XlsxBaseStationsSource() override;

    QStringList pages() override;

private:
    QXlsx::Document* xlsx = nullptr;

protected:
    QVariant read(int row, int column) const override;
    void selectPage(const QString pageName) override;
};

#endif // XLSXBASESTATIONSSOURCE_H
