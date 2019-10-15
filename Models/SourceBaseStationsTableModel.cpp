#include "SourceBaseStationsTableModel.h"
#include <QDebug>
#include <QFont>

SourceBaseStationsTableModel::SourceBaseStationsTableModel(AbstractBaseStationsSource *source,
                                                           int tableRowsCount, int tableColumnsCount,
                                                           QObject *parent)
    : QAbstractTableModel(parent),
      source(source),
      tableRowsCount(tableRowsCount),
      tableColumnsCount(tableColumnsCount)
{
}

int SourceBaseStationsTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return pageData.size();
}

int SourceBaseStationsTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return pageData.size() == 0 ? 0 : pageData.first().size();
}

QVariant SourceBaseStationsTableModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() >= rowCount(index))
    {
        return QVariant();
    }

    switch (role)
    {
        case Qt::DisplayRole:
        {
            int row = index.row();
            int column = index.column();
            QString value = pageData.at(row).at(column);
            bool isInt;
            value.toInt(&isInt);
            if(isInt)
            {
                return value;
            }
            bool isDouble;
            double doubleValue = value.toDouble(&isDouble);
            return isDouble
                    ? QString::number(doubleValue, 'f', 3)
                    : value;
        }
        case Qt::FontRole:
        {
            if(index.row() > 1)
            {
                break;
            }
            QFont font;
            font.setBold(true);
            return font;
        }
        case Qt::TextAlignmentRole:
        {
            if(index.row() != 0)
            {
                break;
            }
            return Qt::AlignCenter;
        }
    }

    return QVariant();
}

void SourceBaseStationsTableModel::update(const QString &pageName)
{
    beginResetModel();
    pageData = source->readAllFromPage(pageName, tableRowsCount, tableColumnsCount);
    endResetModel();
}


QVariant SourceBaseStationsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation != Qt::Horizontal || role != Qt::DisplayRole)
    {
        return QAbstractTableModel::headerData(section, orientation, role);
    }

    return source->columnNames()[section];
}
