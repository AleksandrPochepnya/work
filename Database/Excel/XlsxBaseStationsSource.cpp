#include "XlsxBaseStationsSource.h"

XlsxBaseStationsSource::XlsxBaseStationsSource(const QString& fileName, QObject *parent)
    : ExcelBaseStationsSource(fileName, parent)
{
    xlsx =  new QXlsx::Document(fileName, this);
}

XlsxBaseStationsSource::~XlsxBaseStationsSource()
{
}

QStringList XlsxBaseStationsSource::pages()
{
    return xlsx == nullptr ? QStringList() : xlsx->sheetNames();
}

QVariant XlsxBaseStationsSource::read(int row, int column) const
{
    auto cell = xlsx->cellAt(row, column);
    return cell == nullptr ? QVariant() : cell->value();
}

void XlsxBaseStationsSource::selectPage(const QString pageName)
{
    xlsx->selectSheet(pageName);
}
