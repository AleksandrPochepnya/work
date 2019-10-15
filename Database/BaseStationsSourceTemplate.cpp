#include "BaseStationsSourceTemplate.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QFile>
#include <QDebug>
#include "Excel/ExcelBaseStationsSource.h"

BaseStationsSourceTemplate::BaseStationsSourceTemplate(QObject *parent)
    : QObject(parent)
{
    qDebug() << "ExcelBsSourceTemplate ctor";
    setDefaults();
}

BaseStationsSourceTemplate::BaseStationsSourceTemplate(const QString& fileName, QObject *parent)
    : QObject(parent)
{
    qDebug() << "ExcelBsSourceTemplate ctor" << fileName;
    loadFromFile(fileName);
}


void BaseStationsSourceTemplate::save()
{
    qDebug() << "Save template" << fileName;
    saveAs(fileName);
}

void BaseStationsSourceTemplate::saveAs(const QString &fileName)
{
    qDebug() << "Save template as" << fileName;

    QJsonObject json;
    json["firstRow"] = firstRow;
    json["defaultBsType"] = static_cast<int>(defaultBsType);
    json["useSingleBsType"] = useSingleBsType;
    json["singleBsType"] = static_cast<int>(singleBsType);
    json["pageName"] = pageName;

    QJsonArray bsTypesArray;
    for(const auto key: bsTypesTemplates.keys())
    {
        QJsonObject obj;
        obj["type"] = static_cast<int>(key);
        obj["pattern"] = bsTypesTemplates[key].pattern;
        obj["select"] = bsTypesTemplates[key].select;

        bsTypesArray.append(obj);
    }

    QJsonArray columnsMappingArray;
    auto bsFields = BaseStation::fieldNames();

    for(const auto key: columnsMapping.keys())
    {
        QJsonObject obj;
        obj["field"] = bsFields[key];
        auto options = columnsMapping[key];
        obj["column"] = options.column;
        obj["select"] = options.select;
        if(BaseStation::fieldCanBeNegative(key))
        {
            obj["negative"] = options.negative;
        }
        if(!options.defaultValue.isEmpty())
        {
            obj["default"] = options.defaultValue;
        }

        columnsMappingArray.append(obj);
    }

    json["bsTypes"] = bsTypesArray;
    json["mapping"] = columnsMappingArray;

    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Error:" << file.errorString();
        return;
    }

    file.write(QJsonDocument(json).toJson());
    file.close();
}

void BaseStationsSourceTemplate::remove()
{
    qDebug() << "Remove template" << fileName;
    QFile(fileName).remove();
}

void BaseStationsSourceTemplate::loadFromFile(const QString &fileName)
{
    this->fileName = fileName;

    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Error:" << file.errorString();
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonObject json = QJsonDocument::fromJson(data).object();

    firstRow = json.value("firstRow").toInt(2);
    defaultBsType = static_cast<BaseStation::Type>(json.value("defaultBsType").toInt());
    useSingleBsType = json.value("useSingleBsType").toBool();
    singleBsType = static_cast<BaseStation::Type>(json.value("singleBsType").toInt());
    pageName = json.value("pageName").toString();

    bsTypesTemplates.clear();

    auto columns = ExcelBaseStationsSource::columnNamesList();

    auto bsTypesArray = json.value("bsTypes").toArray();
    for(const auto item : bsTypesArray)
    {
        auto json = item.toObject();
        auto typeValue = json.value("type");
        auto type = typeValue.isString()
                ? static_cast<BaseStation::Type>(columns.indexOf(typeValue.toString()))
                : static_cast<BaseStation::Type>(typeValue.toInt());
        auto pattern = json.value("pattern").toString();
        auto select = json.value("select").toBool();

        bsTypesTemplates[type] = BsTemplateOptions(select, pattern);
    }

    columnsMapping.clear();
    auto columnsMappingArray = json.value("mapping").toArray();
    auto fieldByNameMapping = BaseStation::fieldByNameMapping();
    for(const auto item : columnsMappingArray)
    {
        auto json = item.toObject();
        auto fieldName= json.value("field").toString();
        if(!fieldByNameMapping.contains(fieldName))
        {
            continue;
        }
        auto field = fieldByNameMapping.value(fieldName);
        auto select = json.value("select").toBool();
        auto column = json.value("column").toInt();
        auto negative = json.value("negative").toBool();
        auto defaultValue = json.value("default").toString("-");

        if(!BaseStation::defaultFieldValues()[field].isEmpty())
        {
            if(defaultValue == "-")
            {
                defaultValue = BaseStation::defaultFieldValues()[field];
            }
        }
        else
        {
            defaultValue = "";
        }

        columnsMapping[field] =  ColumnMappingOptions(select, column, negative, defaultValue);
    }

    qDebug() << "BS templates size" << bsTypesTemplates.size();
    qDebug() << "Mapping size" << columnsMapping.size();
}

void BaseStationsSourceTemplate::setDefaults()
{
    firstRow = 1;
    auto fields = BaseStation::fieldNames();
    int column = 1;
    columnsMapping.clear();
    for(const auto& field: fields.keys())
    {
        auto defaultValue = BaseStation::defaultFieldValues()[field];
        columnsMapping.insert(field,  ColumnMappingOptions(true, column++, false, defaultValue));
    }

    useSingleBsType = true;
    defaultBsType = BaseStation::Type::UNKNOWN;

    bsTypesTemplates.clear();
    auto typeNames = BaseStation::typeNames();
    for(const auto type: typeNames.keys())
    {
        bsTypesTemplates.insert(type, BsTemplateOptions(true, typeNames[type]));
    }
}
