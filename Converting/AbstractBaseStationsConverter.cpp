#include "AbstractBaseStationsConverter.h"
#include <QDebug>
#include <cmath>

AbstractBaseStationsConverter::AbstractBaseStationsConverter(AbstractBaseStationsSource *bsSource,
                                                             AbstractDbStorage *dbStorage,
                                                             QObject *parent)
    : QObject(parent),
      bsSource(bsSource),
      dbStorage(dbStorage)
{
    connect(bsSource, &AbstractBaseStationsSource::error, this, [this](int row, QString error){
        qDebug() << "BS source error" << error;
        Q_UNUSED(error);
        QString err = tr("<font size=\"3\" color=\"red\" face=\"Arial\"> "
                         "<strong><em>Error: %1</em></strong> </font> ").arg(error);
        err.toHtmlEscaped();
        addInvalidRow(row, err, true);
    });
}

void AbstractBaseStationsConverter::convert(const QString& bsPage)
{
    emit started();
    mIsRunning = true;
    currentBsPage = bsPage;
    isCanceled = false;

    invalidRowsDataList.clear();
    invalidRowsList.clear();

    int i = 0;
    QList<BaseStation> baseStations = bsSource->readFromPage(bsPage);
    qDebug() << "Base stations count" << baseStations.size();

    int added = 0;
    int updated = 0;

    for(const auto& bs: baseStations)
    {
        if(isCanceled)
        {
            break;
        }

        if((abs(bs.lat) <= 1.0E-6 && abs(bs.lon) <= 1.0E-6)
                || bs.lat < -90 || bs.lat > 90
                || bs.lon < -180 || bs.lon > 180)
        {

            qDebug() << "Cell MCC=" << bs.mcc << "MNC=" << bs.mnc << "LAC=" << bs.lac << "CID="
                     << bs.cellId << "has invalid coordinates (" << bs.lat << ";" << bs.lon << ")";
            QString err = tr("<font size=\"3\" color=\"red\" face=\"Arial\"> "
                             "<strong><em>Error: Lot or Lat went beyond</em></strong> </font> ");
            addInvalidRow(i, err);
        }
        else
        {
            bool result;
            int resultForError = 0;
            if(dbStorage->baseStationExists(bs))
            {
                result = dbStorage->updateBaseStation(bs);
                updated += result ? 1 : 0;
                resultForError = result ? 0 : 1;
            }
            else
            {
                result = dbStorage->addBaseStation(bs);
                added += result ? 1 : 0;
                resultForError = result ? 0 : 2;
            }

            if(!result)
            {
                emit error(i + 1, dbStorage->lastError());
                QString err = "";
                switch (resultForError)
                {
                case 1:
                    err = tr("<font size=\"3\" color=\"red\" face=\"Arial\"> "
                                     "<strong><em>Worning: record has been update</em></strong> </font> ");
                    break;
                case 2:
                    err = tr("<font size=\"3\" color=\"red\" face=\"Arial\"> "
                                     "<strong><em>Worning: record has been added</em></strong> </font> ");
                   break;
                }
                addInvalidRow(i, err);
                break;
            }
        }

        emit progress(static_cast<int>(++i * 100.0 / baseStations.size()));
    }

    emit finished(added, updated, invalidRowsList.size());
    mIsRunning = false;
}

void AbstractBaseStationsConverter::cancel()
{
    isCanceled = true;
}

QStringList AbstractBaseStationsConverter::invalidRowsData() const
{
    return invalidRowsDataList;
}

bool AbstractBaseStationsConverter::isRunning() const
{
    return mIsRunning;
}

void AbstractBaseStationsConverter::addInvalidRow(int row, const QString& err, bool flag )
{
    if (flag)
        row -= bsSource->GetsourceTemplate()->firstRow;
    if(!invalidRowsList.contains(row))
    {
        invalidRowsDataList.append(bsSource->readRowFromPage(currentBsPage, row) + err);
        invalidRowsList.insert(row);
    }
}
