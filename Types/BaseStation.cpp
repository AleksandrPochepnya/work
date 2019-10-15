#include "BaseStation.h"
#include <QObject>
#include <limits>

QList<BaseStation::Field> BaseStation::fieldsOrder()
{
    static QList<BaseStation::Field> fields = {Field::NAME,
                                               Field::TYPE,
                                               Field::MCC,
                                               Field::MNC,
                                               Field::LAC,
                                               Field::CELLID,
                                               Field::CELLIDSUFFIX,
                                               Field::LAT,
                                               Field::LON,
                                               Field::ADDRESS,
                                               Field::AZIMUT,
                                               Field::TURN};
    return fields;
}

QMap<BaseStation::Field, QString> BaseStation::fieldNames()
{
    static QMap<BaseStation::Field, QString> fields = {{Field::MCC,     "MCC"},
                                                       {Field::MNC,     "MNC"},                                                       
                                                       {Field::TYPE,    "TYPE"},
                                                       {Field::LAC,     "LAC"},
                                                       {Field::CELLID,  "CELLID"},
                                                       {Field::CELLIDSUFFIX, "CELLID SUFFIX"},
                                                       {Field::NAME,    "NAME"},
                                                       {Field::LAT,     "LAT"},
                                                       {Field::LON,     "LON"},
                                                       {Field::ADDRESS, "ADDRESS"},
                                                       {Field::AZIMUT,  "AZIMUT"},
                                                       {Field::TURN,    "TURN"}};
    return fields;
}

QMap<BaseStation::Field, QString> BaseStation::fieldDescriptions()
{
    static QMap<BaseStation::Field, QString> fields = {{Field::MCC,     QObject::tr("Country code")},
                                                       {Field::MNC,     QObject::tr("Network code")},
                                                       {Field::LAC,     QObject::tr("Location code")},
                                                       {Field::CELLID,  QObject::tr("Cell identifier")},
                                                       {Field::CELLIDSUFFIX,  QObject::tr("Cell identifier suffix")},
                                                       {Field::NAME,    QObject::tr("Base station name")},
                                                       {Field::LAT,     QObject::tr("Latitude")},
                                                       {Field::LON,     QObject::tr("Longitude")},
                                                       {Field::ADDRESS, QObject::tr("Base station address")},
                                                       {Field::AZIMUT,  QObject::tr("Antenna direction azimut")},
                                                       {Field::TURN,    QObject::tr("Turn of antenna")},
                                                       {Field::TYPE,    QObject::tr("Base station type")}};
    return fields;
}

QMap<BaseStation::Field, QString> BaseStation::defaultFieldValues()
{
    static QMap<BaseStation::Field, QString> fields = {{Field::MCC,     "255"},
                                                       {Field::MNC,     "1"},
                                                       {Field::LAC,     ""},
                                                       {Field::CELLID,  ""},
                                                       {Field::NAME,    ""},
                                                       {Field::LAT,     ""},
                                                       {Field::LON,     ""},
                                                       {Field::ADDRESS, ""},
                                                       {Field::AZIMUT,  "0"},
                                                       {Field::TURN,    "120"},
                                                       {Field::TYPE,    ""}};
    return fields;
}

QMap<BaseStation::Field, std::pair<int, int> > BaseStation::fieldValuesRanges()
{
    static QMap<BaseStation::Field, std::pair<int, int> > ranges = {{Field::MCC, std::make_pair(0, INT_MAX)},
                                                                   {Field::MNC,  std::make_pair(0, INT_MAX)},
                                                                   {Field::TURN, std::make_pair(0, 360)},
                                                                   {Field::AZIMUT, std::make_pair(0, 360)}};
    return ranges;
}

QMap<QString, BaseStation::Field> BaseStation::fieldByNameMapping()
{
    static QMap<QString, BaseStation::Field> fields = {{"MCC",     Field::MCC},
                                                       {"MNC",     Field::MNC},
                                                       {"LAC",     Field::LAC},
                                                       {"CELLID",  Field::CELLID},
                                                       {"CELLID SUFFIX", Field::CELLIDSUFFIX},
                                                       {"NAME",    Field::NAME},
                                                       {"LAT",     Field::LAT},
                                                       {"LON",     Field::LON},
                                                       {"ADDRESS", Field::ADDRESS},
                                                       {"AZIMUT",  Field::AZIMUT},
                                                       {"TURN",    Field::TURN},
                                                       {"TYPE",    Field::TYPE}};
    return fields;
}

bool BaseStation::fieldCanBeNegative(BaseStation::Field field)
{
    return field == BaseStation::Field::LAT || field == BaseStation::Field::LON;
}

QMap<BaseStation::Type, QString> BaseStation::typeNames()
{
    static QMap<Type, QString> names = {{Type::UNKNOWN, "UNKNOWN"},
                                        {Type::GSM,     "GSM"},
                                        {Type::UMTS,    "UMTS"},
                                        {Type::LTE,     "LTE"},
                                        {Type::CDMA,    "CDMA"},
                                        {Type::TYPE5G,  "5G"}};
    return names;
}
