#include "ExcelBaseStationsSource.h"

ExcelBaseStationsSource::ExcelBaseStationsSource(const QString& fileName, QObject *parent)
    : AbstractBaseStationsSource(parent), fileName(fileName)
{
}

ExcelBaseStationsSource::~ExcelBaseStationsSource()
{
}

QStringList ExcelBaseStationsSource::columnNamesList()
{
    static QStringList names;

    static bool init = false;
    if(init)
    {
        return names;
    }

    QStringList letters;
    for(int i = 0; i < 26; ++i)
    {
        letters.append(QString('A' + i));
    }

    for(int i = 0; i < 100; ++i)
    {
        if(i < letters.size())
        {
            names.append(letters[i]);
        }
        else
        {
            int n = letters.size();
            names.append(letters[i / n - 1] + letters[i % n]);
        }
    }
    init = true;

    return names;
}

QStringList ExcelBaseStationsSource::columnNames() const
{
   return columnNamesList();
}
