#include "ApplicationSettings.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QFile>
#include <QDebug>

ApplicationSettings::ApplicationSettings() : QObject(nullptr)
{
    if(!load())
    {
        save();
    }
}

ApplicationSettings &ApplicationSettings::instance()
{
    static ApplicationSettings settings;
    return settings;
}

void ApplicationSettings::save()
{
    qDebug() << "Settings saving";

    QJsonObject json;
    json["language"] = language;
    json["database"] = dbFileName;
    json["empty_database"] = emptyDbFileName;
    json["previewRowsCount"] = previewRowsCount;
    json["previewColumnsCount"] = previewColumnsCount;
    json["lastUsedBsFile"] = lastUsedBsFile;
    json["lastUsedTemplate"] = lastUsedTemplate;
    json["updateDbAfterConverting"] = updateDbAfterConverting;

    QFile file(SettingsFileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Error:" << file.errorString();
        return;
    }

    file.write(QJsonDocument(json).toJson());
    file.close();

    qDebug() << "Settings saved";
    print();
}

bool ApplicationSettings::load()
{
    qDebug() << "Settings loading";

    QFile file(SettingsFileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Error:" << file.errorString();
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonObject json = QJsonDocument::fromJson(data).object();

    language = json.value("language").toString("ru");
    dbFileName = json.value("database").toString("stations.db");
    emptyDbFileName = json.value("empty_database").toString("stations_empty.db");
    previewRowsCount = json.value("previewRowsCount").toInt(50);
    previewColumnsCount = json.value("previewColumnsCount").toInt(20);
    lastUsedBsFile = json.value("lastUsedBsFile").toString();
    lastUsedTemplate = json.value("lastUsedTemplate").toString();
    updateDbAfterConverting = json.value("updateDbAfterConverting").toBool(true);

    qDebug() << "Settings loaded";
    print();

    return true;
}

void ApplicationSettings::print()
{
    qDebug() << "language =" << language;
    qDebug() << "database =" << dbFileName;
    qDebug() << "empty database =" << emptyDbFileName;
    qDebug() << "preview rows count =" << previewRowsCount;
    qDebug() << "preview columns count =" << previewColumnsCount;
    qDebug() << "last used bs file =" << lastUsedBsFile;
    qDebug() << "last used template =" << lastUsedTemplate;
    qDebug() << "update DB after converting" << updateDbAfterConverting;
}
