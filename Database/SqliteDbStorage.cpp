#include "SqliteDbStorage.h"
#include <QDebug>
#include <QFileInfo>

SqliteDbStorage::SqliteDbStorage(const QString &dbFileName, QObject *parent)
    : AbstractDbStorage(parent), dbFileName(dbFileName)
{
}

SqliteDbStorage::~SqliteDbStorage()
{
    QString oldConnectionName = db.connectionName();
    db.close();
    db = QSqlDatabase();
    QSqlDatabase::removeDatabase(oldConnectionName);
}

bool SqliteDbStorage::baseStationExists(const BaseStation &bs)
{
    int cellId = bs.cellId;

    if(!bs.cellIdSuffix.isEmpty())
    {
        cellId = QString(QString::number(cellId) + bs.cellIdSuffix).toInt();
    }

    int res = executeSingleResultQuery(QString("SELECT COUNT(*) FROM CELLS AS c "
                                               "INNER JOIN OPERATORS AS o ON o.OPERATORID = c.OPERATORID "
                                               "WHERE o.MCC=%1 AND o.MNC=%2 AND c.LAC=%3 AND c.CELLID=%4")
                                       .arg(bs.mcc)
                                       .arg(bs.mnc)
                                       .arg(bs.lac)
                                       .arg(cellId))
            .toInt();
    return res != 0;
}

bool SqliteDbStorage::addBaseStation(const BaseStation &bs)
{
    int res = executeQuery(QString("INSERT INTO CELLS (LAC, CELLID, LAT, LON, ADR, ANGLE, TURN, OPERATORID, STANDARDID, CCO, LID, CELL_NAME) "
                                   "VALUES(%1, %2, %3, %4, '%5', %6, %7, %8, %9, %10, %11, '%12')")
                           .arg(bs.lac)
                           .arg(bs.cellId)
                           .arg(bs.lat)
                           .arg(bs.lon)
                           .arg(bs.address)
                           .arg(bs.azimut)
                           .arg(bs.turn)
                           .arg(findOperator(bs.mcc, bs.mnc))
                           .arg(static_cast<int>(bs.type))
                           .arg(bs.cco)
                           .arg(bs.lid)
                           .arg(bs.name), QueryType::Insert);
    return res != -1;
}

bool SqliteDbStorage::updateBaseStation(const BaseStation &bs)
{
    int res = executeQuery(QString("UPDATE CELLS SET LAT=%1, LON=%2, ADR='%3', ANGLE=%4, TURN=%5, STANDARDID=%6, CCO=%7, LID=%8, CELL_NAME='%9' "
                                   "WHERE LAC=%10 AND CELLID=%11 AND EXISTS (SELECT * FROM OPERATORS AS o WHERE o.MCC=%12 AND o.MNC=%13)")
                           .arg(bs.lat)
                           .arg(bs.lon)
                           .arg(bs.address)
                           .arg(bs.azimut)
                           .arg(bs.turn)
                           .arg(static_cast<int>(bs.type))
                           .arg(bs.cco)
                           .arg(bs.lid)
                           .arg(bs.name)
                           .arg(bs.lac)
                           .arg(bs.cellId)
                           .arg(bs.mcc)
                           .arg(bs.mnc), QueryType::Update);
    return res == 0;
}

int SqliteDbStorage::findOperator(int mcc, int mnc)
{
    auto res = executeSingleResultQuery(QString("SELECT OPERATORID FROM OPERATORS WHERE MCC=%1 AND MNC=%2")
                                        .arg(mcc)
                                        .arg(mnc));
    return res.isNull() ? addOperator(mcc, mnc) : res.toInt();
}

int SqliteDbStorage::addOperator(int mcc, int mnc)
{
    int operatorId = executeSingleResultQuery("SELECT MAX(OPERATORID) FROM OPERATORS").toInt() + 1;

    return executeQuery(QString("INSERT INTO OPERATORS (OPERATORID, MCC, MNC, NAME, FULLNAME) "
                                "VALUES (%1, %2, %3, 'Unknown', 'Unknown')")
                        .arg(operatorId)
                        .arg(mcc)
                        .arg(mnc), QueryType::Insert);
}

int SqliteDbStorage::executeQuery(const QString &queryText, QueryType type)
{
//    qDebug() << "Query: " << queryText;

    if(!db.isOpen())
    {
        if(!db.open())
        {
            lastErrorMessage = db.lastError().databaseText();
            qDebug() << "Database not opened" << lastErrorMessage;
            return -1;
        }
    }
    QSqlQuery query(db);
    if(!query.exec(queryText))
    {
        auto error = query.lastError();
        lastErrorMessage = error.databaseText();
        qDebug() << "Query error: " << lastErrorMessage;
        return -1;
    }

    return type == QueryType::Insert
            ? query.lastInsertId().toInt()
            : 0;
}

QVariant SqliteDbStorage::executeSingleResultQuery(const QString &queryText)
{
//    qDebug() << "Query: " << queryText;

    if(!db.isOpen())
    {
        if(!db.open())
        {
            lastErrorMessage = db.lastError().databaseText();
            qDebug() << "Database not opened" << lastErrorMessage;
            return QVariant();
        }
    }

    QSqlQuery query(db);
    if(!query.exec(queryText))
    {
        auto error = query.lastError();
        lastErrorMessage = error.databaseText();
        qDebug() << "Query error: " << lastErrorMessage;
        return QVariant();
    }

    if(!query.next())
    {
        lastErrorMessage = db.lastError().databaseText();
        qDebug() << "No result retrieved";
        return QVariant();
    }

    return query.value(0);
}

int SqliteDbStorage::getVersion()
{
    qDebug() << "Getting version";
    return containsTable("system_info")
            ? executeSingleResultQuery("SELECT value FROM system_info WHERE key='db_version'").toString().toInt()
            : 1;
}

bool SqliteDbStorage::setVersion(int version)
{
    qDebug() << "Setting version" << version;
    return executeQuery(QString("UPDATE system_info SET value = %1 WHERE key = 'db_version'").arg(version), QueryType::Update) == 0;
}

bool SqliteDbStorage::updateStorage()
{
    qDebug() << "Storage updating";

    int version = getVersion();
    qDebug() << "Storage version" << version;

    bool result = false;
    switch(version)
    {
        case 1:
            result = updateStorageV2() && updateStorageV3();
            break;
        case 2:
            result = updateStorageV3();
            return true;
        case 3:
            qDebug() << "Storage is already up to date";
            return true;
        default:
            qDebug() << "Unknown version" << version;
            return false;
    }

    if(!result)
        return false;

    return setVersion(lastVersion);
}

bool SqliteDbStorage::updateStorageV2()
{
    qDebug() << "Updating storage to version 2";

    return executeQuery("CREATE TABLE IF NOT EXISTS `system_info` ("
                            "`key`    TEXT NOT NULL PRIMARY KEY UNIQUE, "
                            "`value`  TEXT NOT NULL )", QueryType::Update) == 0

        && executeQuery("INSERT INTO system_info(key, value) "
                        "SELECT 'db_version', '2' "
                        "WHERE NOT EXISTS(SELECT 1 FROM system_info WHERE key = 'db_version')", QueryType::Insert) != -1

        && executeQuery("ALTER TABLE CELLS ADD COLUMN CELL_NAME TEXT", QueryType::Update) == 0

            && executeQuery("CREATE INDEX `CELL_NAME_IND` ON `CELLS` ( `CELL_NAME` ASC )", QueryType::Update) == 0;
}

bool SqliteDbStorage::updateStorageV3()
{
    qDebug() << "Updating storage to version 3";

    return executeQuery("INSERT INTO OPERATORS (OPERATORID, MCC, MNC, NAME, FULLNAME) VALUES((SELECT max(OPERATORID) + 1 FROM OPERATORS), 434,6,'perfectum', 'perfectum')", QueryType::Insert) != -1 &&
           executeQuery("INSERT INTO OPERATORS (OPERATORID, MCC, MNC, NAME, FULLNAME) VALUES((SELECT max(OPERATORID) + 1 FROM OPERATORS), 434,3,'Uzmobile cdma', 'Uzmobile cdma')", QueryType::Insert) != -1 &&
           executeQuery("UPDATE system_info SET value='3' WHERE key='db_version'", QueryType::Update) == 0;
}

bool SqliteDbStorage::containsField(const QString &tableName, const QString &fieldName)
{
    qDebug() << "Checking if table" << tableName << "contains field" << fieldName;

    QString queryText = QString("PRAGMA table_info('%1')").arg(tableName);
    qDebug() << "Query: " << queryText;

    if(!db.isOpen())
    {
        if(!db.open())
        {
            lastErrorMessage = db.lastError().databaseText();
            qDebug() << "Database not opened" << lastErrorMessage;
            return false;
        }
    }

    QSqlQuery query(db);
    if(!query.exec(queryText))
    {
        auto error = query.lastError();
        lastErrorMessage = error.databaseText();
        qDebug() << "Query error: " << lastErrorMessage;
        return false;
    }

    while(query.next())
    {
        if(query.value("name").toString() == fieldName)
        {
            return true;
        }
    }

    return false;
}

bool SqliteDbStorage::containsTable(const QString &tableName)
{
    qDebug() << "Checking if database contains table" << tableName;
    return !executeSingleResultQuery(QString("SELECT 1 FROM sqlite_master WHERE type='table' AND name='%1'").arg(tableName)).isNull();
}

bool SqliteDbStorage::open()
{
    db = QSqlDatabase::addDatabase("QSQLITE", QFileInfo(dbFileName).baseName());
    db.setDatabaseName(dbFileName);
    if (!db.open())
    {
        qDebug() << "Database not open" << db.lastError().text();
        lastErrorMessage = db.lastError().text();
        return false;
    }

    return true;
}


bool SqliteDbStorage::init()
{
    bool result = updateStorage();
    if(result)
    {
        lastErrorMessage = "Storage updated successfully";
    }
    else
    {
        lastErrorMessage = tr("Cannot update storage. Check log to see more info");
    }

    qDebug() << lastErrorMessage;

    return result;
}
