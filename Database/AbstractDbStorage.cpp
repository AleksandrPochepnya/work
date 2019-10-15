#include "AbstractDbStorage.h"

QString AbstractDbStorage::lastError() const
{
    return lastErrorMessage;
}

AbstractDbStorage::AbstractDbStorage(QObject *parent)
    : QObject(parent)
{

}
