#ifndef BASESTATION_H
#define BASESTATION_H

#include <QMap>
#include <QList>
#include <utility>

struct BaseStation
{
    enum class Field
    {
        NAME,
        MCC,
        MNC,
        LAC,
        CELLID,
        LAT,
        LON,
        ADDRESS,
        AZIMUT,
        TURN,
        TYPE,
        CELLIDSUFFIX
    };

    enum class Type
    {
        UNKNOWN,
        GSM,
        UMTS,
        CDMA,
        LTE,
        TYPE5G
    };

    QString address = "";
    int     azimut = 0;
    int     cco = 0;
    int     cellId = 0;
    int     lac = 0;
    double  lat = 0;
    int     lid = 0;
    double  lon = 0;
    int     mcc = 0;
    int     mnc = 0;
    QString name = "";
    int     turn = 0;
    Type    type = Type::UNKNOWN;
    QString cellIdSuffix;

    static QList<Field> fieldsOrder();
    static QMap<Field, QString> fieldNames();
    static QMap<Field, QString> fieldDescriptions();
    static QMap<Field, QString> defaultFieldValues();
    static QMap<Field, std::pair<int, int>> fieldValuesRanges();
    static QMap<QString, Field> fieldByNameMapping();
    static bool fieldCanBeNegative(Field field);

    static QMap<Type, QString> typeNames();
};

#endif // BASESTATION_H
