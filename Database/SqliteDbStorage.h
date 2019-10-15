#ifndef SQLITEDBSTORAGE_H
#define SQLITEDBSTORAGE_H

#include "AbstractDbStorage.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

class SqliteDbStorage : public AbstractDbStorage
{
    Q_OBJECT
public:
    explicit SqliteDbStorage(const QString& dbFileName, QObject *parent = nullptr);
    ~SqliteDbStorage() override;

    bool open() override;
    bool init() override;

    bool baseStationExists(const BaseStation &bs) override;
    bool addBaseStation(const BaseStation &bs) override;
    bool updateBaseStation(const BaseStation &bs) override;

private:
    QSqlDatabase db;
    QString dbFileName;
    const int lastVersion = 2;

    enum class QueryType {
          Insert,
          Delete,
          Update
      };

    int findOperator(int mcc, int mnc);
    int addOperator(int mcc, int mnc);

    /* Если type = Insert: -1 - ошибка, иначе - id новой записи
       Если type = Delete или Update: -1 - ошибка, 0 - все ок
    */
    int executeQuery(const QString& queryText, QueryType type);
    QVariant executeSingleResultQuery(const QString &queryText);   

    int getVersion();
    bool setVersion(int version);
    bool updateStorage();
    bool updateStorageV2();
    bool updateStorageV3();

    bool containsField(const QString& tableName, const QString& fieldName);    
    bool containsTable(const QString& tableName);
};

#endif // SQLITEDBSTORAGE_H
