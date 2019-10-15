#ifndef APPLICATIONSETTINGS_H
#define APPLICATIONSETTINGS_H

#include <QObject>

class ApplicationSettings : public QObject
{
    Q_OBJECT
public:
    static ApplicationSettings& instance();
    void save();

    QString language = "ru";
    QString dbFileName = "stations.db";
    QString emptyDbFileName = "stations_empty.db";
    int previewColumnsCount = 20;
    int previewRowsCount = 50;
    QString lastUsedBsFile = "";
    QString lastUsedTemplate = "";
    bool updateDbAfterConverting = true;

private:
    explicit ApplicationSettings();

    const QString SettingsFileName = "settings.json";

    bool load();

    void print();
};

#endif // APPLICATIONSETTINGS_H
