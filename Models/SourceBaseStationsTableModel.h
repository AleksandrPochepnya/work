#ifndef SOURCEBASESTATIONSTABLEMODEL_H
#define SOURCEBASESTATIONSTABLEMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include "Database/AbstractBaseStationsSource.h"

class SourceBaseStationsTableModel: public QAbstractTableModel
{
public:
    SourceBaseStationsTableModel(AbstractBaseStationsSource* source, int tableRowsCount, int tableColumnsCount, QObject* parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void update(const QString& pageName);

private:
    AbstractBaseStationsSource* source;
    int tableRowsCount;
    int tableColumnsCount;

    QList<QStringList> pageData;
};

#endif // SOURCEBASESTATIONSTABLEMODEL_H
