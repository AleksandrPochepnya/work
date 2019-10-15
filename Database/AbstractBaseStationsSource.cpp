#include "AbstractBaseStationsSource.h"

AbstractBaseStationsSource::AbstractBaseStationsSource(QObject *parent)
    : QObject(parent)
{
    mCoordinatesRegExp = QRegExp("[0-9]{1,2}[°|˚][0-9]{1,2}['][0-9]+[.]?[0-9]*\"?[NSEW]?");
    mNumberRegExp = QRegExp("[0-9]+[.]?[0-9]*");
}

QString AbstractBaseStationsSource::readRowFromPage(const QString &pageName, int row)
{
    selectPage(pageName);

    row += sourceTemplate->firstRow;

    QString res = QString("%1\t").arg(row);

    int columnsCount = 20;
    for(int column = 1; column <= columnsCount; ++column)
    {
        res += read(row, column).toString();

        if(column < columnsCount)
        {
            res += "; ";
        }
    }

    return res;
}

QList<BaseStation> AbstractBaseStationsSource::readFromPage(const QString &pageName)
{
    selectPage(pageName);

    QList<BaseStation> baseStations;
    auto& m = sourceTemplate->columnsMapping;

    bool ok = false;
    for(int row = sourceTemplate->firstRow; ; ++row)
    {
        if(isRowEmpty(row))
        {
            break;
        }

        BaseStation bs;

        if(m[BaseStation::Field::MCC].select)
        {
            bs.mcc = read(row, m[BaseStation::Field::MCC].column).toInt(&ok);
            if(!ok)
            {
                onError(BaseStation::Field::MCC, row);
            }
        }
        else
        {
            bs.mcc = m[BaseStation::Field::MCC].defaultValue.toInt();
        }

        if(m[BaseStation::Field::MNC].select)
        {
            bs.mnc = read(row, m[BaseStation::Field::MNC].column).toInt(&ok);
            if(!ok)
            {
                onError(BaseStation::Field::MNC, row);
            }
        }
        else
        {
            bs.mnc = m[BaseStation::Field::MNC].defaultValue.toInt();
        }

        if(m[BaseStation::Field::LAC].select)
        {
            bs.lac = read(row, m[BaseStation::Field::LAC].column).toInt(&ok);
            if(!ok)
            {
                onError(BaseStation::Field::LAC, row);
            }
        }

        if(m[BaseStation::Field::CELLID].select)
        {
            bs.cellId = read(row, m[BaseStation::Field::CELLID].column).toInt(&ok);
            if(!ok)
            {
                onError(BaseStation::Field::CELLID, row);
            }
        }

        if(m[BaseStation::Field::LAT].select)
        {
            bs.lat = parseGpsCoordinate(BaseStation::Field::LAT, read(row, m[BaseStation::Field::LAT].column).toString().trimmed().replace(" ", ""), row);
        }

        if(m[BaseStation::Field::LON].select)
        {
            bs.lon = parseGpsCoordinate(BaseStation::Field::LON, read(row, m[BaseStation::Field::LON].column).toString().trimmed().replace(" ", ""), row);
        }

        if(m[BaseStation::Field::ADDRESS].select)
        {
            bs.address = read(row, m[BaseStation::Field::ADDRESS].column).toString().replace("'", "''");
        }

        if(m[BaseStation::Field::AZIMUT].select)
        {
            bs.azimut = read(row, m[BaseStation::Field::AZIMUT].column).toInt(&ok);
            if(!ok)
            {
                onError(BaseStation::Field::AZIMUT, row);
            }
            auto range = BaseStation::fieldValuesRanges().value(BaseStation::Field::AZIMUT);
            if(bs.azimut < range.first  || bs.azimut > range.second)
            {
                qWarning() << "Invalid AZIMUT" << bs.azimut << "Set default" << m[BaseStation::Field::AZIMUT].defaultValue.toInt();
                bs.azimut = m[BaseStation::Field::AZIMUT].defaultValue.toInt();
            }
        }

        if(m[BaseStation::Field::TURN].select)
        {
            bs.turn = read(row, m[BaseStation::Field::TURN].column).toInt(&ok);
            if(!ok)
            {
                onError(BaseStation::Field::TURN, row);
            }
            auto range = BaseStation::fieldValuesRanges().value(BaseStation::Field::TURN);
            if(bs.turn < range.first  || bs.turn > range.second)
            {
                qWarning() << "Invalid TURN" << bs.turn << "Set default" << m[BaseStation::Field::TURN].defaultValue.toInt();
                bs.turn = m[BaseStation::Field::TURN].defaultValue.toInt();
            }
        }
        else
        {
            bs.turn = m[BaseStation::Field::TURN].defaultValue.toInt();
        }

        if(m[BaseStation::Field::NAME].select)
        {
            bs.name = read(row, m[BaseStation::Field::NAME].column).toString();
        }

        if(m[BaseStation::Field::CELLIDSUFFIX].select)
        {
            bs.cellIdSuffix = read(row, m[BaseStation::Field::CELLIDSUFFIX].column).toString();
        }

        BaseStation::Type type = BaseStation::Type::UNKNOWN;
        if(sourceTemplate->useSingleBsType)
        {
            type = sourceTemplate->singleBsType;
        }
        else
        {
            QString typeValue = read(row, m[BaseStation::Field::TYPE].column).toString();
            bool typeFound = false;
            for(auto bsType: sourceTemplate->bsTypesTemplates.keys())
            {
                auto options = sourceTemplate->bsTypesTemplates[bsType];
                if(!options.select)
                {
                    continue;
                }

                qDebug() << BaseStation::typeNames()[bsType] << ": Pattern =" << options.pattern;

                QStringList values = options.pattern.split(';');
                for(auto value: values)
                {
                    value = value.trimmed();

                    if(value == "")
                    {
                        continue;
                    }

                    if(typeValue.contains(value))
                    {
                        qDebug() << "Name contains value" << value;
                        type = bsType;
                        typeFound = true;
                        break;
                    }
                }

                if(typeFound)
                {
                    break;
                }
            }

            if(!typeFound)
            {
                type = sourceTemplate->defaultBsType;
            }
        }

        bs.type = type;
        baseStations.append(bs);
    }

    return baseStations;
}


QList<QStringList> AbstractBaseStationsSource::readAllFromPage(const QString &pageName, int rowsCount, int columnsCount)
{
    qDebug() << "Read" << rowsCount << "rows and" << columnsCount << "columns from page" << pageName;
    selectPage(pageName);

    QList<QStringList> data;

    for(int row = 1; row <= rowsCount; ++row)
    {
        QStringList values;
        for(int column = 1; column <= columnsCount; ++column)
        {
            values.append(read(row, column).toString());
        }

        data.append(values);
    }

    return data;
}

void AbstractBaseStationsSource::setTemplate(const std::shared_ptr<BaseStationsSourceTemplate> &value)
{
    sourceTemplate = value;
}

std::shared_ptr<BaseStationsSourceTemplate> AbstractBaseStationsSource::GetsourceTemplate()
{
    return sourceTemplate;
}

void AbstractBaseStationsSource::onError(BaseStation::Field field, int row) const
{
    int column = sourceTemplate->columnsMapping[field].column;
    auto fieldName = BaseStation::fieldNames()[field];
    auto msg = QString(tr("Invalid %1: row = %2, column = %3, value \"%4\""))
            .arg(fieldName)
            .arg(row)
            .arg(columnNames().at(column - 1))
            .arg(read(row, column).toString());
    qWarning() << msg;
    emit error(row, msg);
}

bool AbstractBaseStationsSource::isRowEmpty(int row) const
{
    for(int column = 1; column < 20; ++column)
    {
        if(!read(row, column).toString().isEmpty())
        {
            return false;
        }
    }

    return true;
}

double AbstractBaseStationsSource::parseGpsCoordinate(BaseStation::Field field, QString text, int row)
{
    double value = 0;
    auto& m = sourceTemplate->columnsMapping;

    if(mCoordinatesRegExp.exactMatch(text))
    {
        int pos = 0;
        QList<double> values;
        while((pos = mNumberRegExp.indexIn(text, pos)) != -1)
        {
            values.append(mNumberRegExp.cap().toDouble());
            pos += mNumberRegExp.matchedLength() + 1;
        }
        if(values.size() != 3)
        {
            onError(field, row);
        }
        else
        {
            value = abs(values.at(0)) + values.at(1) / 60 + values.at(2) / 3600;
            if(values.at(0) < 0)
            {
                value *= -1;
            }

            if(m[field].negative && value > 0)
            {
                value *= -1;
            }
            else if(!m[field].negative && value < 0)
            {
                value *= -1;
            }
        }
    }
    else
    {
        text = text.replace(',', '.');
        while (text.contains(".."))
        {
            text.replace("..", ".");
        }

        bool ok;
        value = text.toDouble(&ok);
        if(!ok)
        {
            onError(field, row);
        }
        else
        {
            if(m[field].negative && value > 0)
            {
                value *= -1;
            }
            else if(!m[BaseStation::Field::LON].negative && value < 0)
            {
                value *= -1;
            }
        }
    }

//     qDebug() << text << "->" << value;

    return value;
}
