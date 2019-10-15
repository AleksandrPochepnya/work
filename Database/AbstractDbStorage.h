#ifndef ABSTRACTDBSTORAGE_H
#define ABSTRACTDBSTORAGE_H

#include <QObject>
#include "Types/BaseStation.h"
#include <QStringList>

class AbstractDbStorage: public QObject
{
public:
    virtual ~AbstractDbStorage() = default;

    virtual bool open() = 0;
    virtual bool init() = 0;

    virtual bool baseStationExists(const BaseStation& bs) = 0;
    virtual bool addBaseStation(const BaseStation& bs) = 0;
    virtual bool updateBaseStation(const BaseStation& bs) = 0;

    QString lastError() const;

protected:
    AbstractDbStorage(QObject* parent = nullptr);

    QString lastErrorMessage;
};

#endif // ABSTRACTDBSTORAGE_H
