#ifndef EXCELBSSOURCETEMPLATE_H
#define EXCELBSSOURCETEMPLATE_H

#include <QObject>
#include "Types/BaseStation.h"

struct BsTemplateOptions
{
    bool select = false;
    QString pattern = "";

    BsTemplateOptions() = default;

    BsTemplateOptions(bool select, QString& pattern)
        : select(select), pattern(pattern) {}
};

struct ColumnMappingOptions
{
    bool select = false;
    int column = 1;
    bool negative = false;
    QString defaultValue = "";

    ColumnMappingOptions() = default;

    ColumnMappingOptions(bool select, int column, bool negative, QString defaultValue = "")
        : select(select), column(column), negative(negative), defaultValue(defaultValue) {}
};


class BaseStationsSourceTemplate: public QObject
{
public:
    BaseStationsSourceTemplate(QObject* parent = nullptr);
    BaseStationsSourceTemplate(const QString& fileName, QObject* parent = nullptr);

    void save();
    void saveAs(const QString& fileName);
    void remove();
    void loadFromFile(const QString& fileName);
    void setDefaults();

    QString fileName;

    int firstRow = 0;
    bool useSingleBsType = true;
    BaseStation::Type defaultBsType = BaseStation::Type::UNKNOWN;
    BaseStation::Type singleBsType = BaseStation::Type::UNKNOWN;
    QString pageName;

    // base station type => pattern
    QMap<BaseStation::Type, BsTemplateOptions> bsTypesTemplates;

    // base station field enum => excel column index
    QMap<BaseStation::Field, ColumnMappingOptions> columnsMapping;
};

#endif // EXCELBSSOURCETEMPLATE_H
