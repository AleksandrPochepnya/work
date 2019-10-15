#include "XlsBaseStationsSource.h"
#include <iostream>

XlsBaseStationsSource::XlsBaseStationsSource(const QString &fileName, QObject *parent)
    : ExcelBaseStationsSource(fileName, parent)
{
    xls = new YExcel::BasicExcel();
    xls->Load(fileName.toStdString().c_str());
}

XlsBaseStationsSource::~XlsBaseStationsSource()
{
    delete xls;
}

QStringList XlsBaseStationsSource::pages()
{
    if(pagesList.size() != 0)
    {
        return pagesList;
    }

    char* name = new char[32];
    wchar_t* wname = new wchar_t[32];

    for(size_t i = 0; i < xls->GetTotalWorkSheets(); ++i)
    {
        QString page;
        if(xls->GetSheetName(i, name))
        {
            page = QString(name);
        }
        else
        {
            xls->GetSheetName(i, wname);
            page = QString::fromWCharArray(wname);
        }

        pagesList.append(page);
    }

    delete[] name;
    delete[] wname;

    return pagesList;
}

QVariant XlsBaseStationsSource::read(int row, int column) const
{
    auto cell = sheet->Cell(row - 1, column - 1);
    int type = cell->Type();
    switch (type)
    {
        case YExcel::BasicExcelCell::INT:     return cell->GetInteger();
        case YExcel::BasicExcelCell::DOUBLE:  return cell->GetDouble();
        case YExcel::BasicExcelCell::STRING:  return cell->GetString();
        case YExcel::BasicExcelCell::WSTRING: return QString::fromWCharArray(cell->GetWString());
        default:                              return QVariant();
    }
}

void XlsBaseStationsSource::selectPage(const QString pageName)
{
    int index = pages().indexOf(pageName);
    if(index == -1)
    {
        qDebug() << "Unknown page" << pageName;
        return;
    }
    sheet = xls->GetWorksheet(index);
}
