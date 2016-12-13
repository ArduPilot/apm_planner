/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2016 APM_PLANNER PROJECT <http://www.ardupilot.com>

This file is part of the APM_PLANNER project

    APM_PLANNER is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    APM_PLANNER is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with APM_PLANNER. If not, see <http://www.gnu.org/licenses/>.

======================================================================*/
/**
 * @file LogdataStorage.cpp
 * @author Arne Wischmann <wischmann-a@gmx.de>
 * @date 16 Nov 2016
 * @brief File providing implementation for the data storage for logfiles
 */


#include "LogdataStorage.h"
#include "logging.h"
#include <algorithm>

/**
 * @brief The TimeStampToIndexPairComparer class is a functor for sorting the
 *        time index by time.
 */
class TimeStampToIndexPairComparer
{
public:
  bool operator() (LogdataStorage::TimeStampToIndexPair pair1, LogdataStorage::TimeStampToIndexPair pair2)
  {
      // pair.first is the time stamp
      return (pair1.first < pair2.first);
  }
};

//****************************************************

LogdataStorage::LogdataStorage() :
    m_columCount(0),
    m_currentRow(0),
    m_timeDivisor(0.0),
    m_minTimeStamp(ULLONG_MAX),
    m_maxTimeStamp(0)
{
    QLOG_DEBUG() << "LogdataStorage::LogdataStorage()";
    // Reserve some memory...
    m_typeStorage.reserve(50);
    m_indexToTypeRow.reserve(50);
    m_dataStorage.reserve(50);
    m_TimeToIndexList.reserve(20000);
    m_indexToDataRow.reserve(20000);
}

LogdataStorage::~LogdataStorage()
{
    QLOG_DEBUG() << "LogdataStorage::~LogdataStorage()";
}

int LogdataStorage::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_indexToDataRow.size();
}

int LogdataStorage::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_columCount;
}

QVariant LogdataStorage::data(const QModelIndex &index, int role) const
{
    if ((role != Qt::DisplayRole) || !index.isValid())
    {
        return QVariant();
    }
    if (index.row() >= m_indexToDataRow.size())
    {
        QLOG_ERROR() << "Accessing a row that does not exist! Row was: " << index.row();
        return QVariant();
    }
    if (index.column() == 0)
    {
        // Column 0 is the index of the log data which is the same as the row
        return QVariant(QString::number(index.row()));
    }
    if (index.column() == 1)
    {
        // Column 1 is the name of the log data (ATT,ATUN...)
        return QVariant(m_indexToDataRow[index.row()].first);
    }
    const TypeIndexPair &typeIndex = m_indexToDataRow[index.row()];
    if(index.column() - s_ColumnOffset >= m_dataStorage[typeIndex.first].at(typeIndex.second).m_values.size())
    {
        return QVariant(); // this data type does not have so much colums
    }
    return QVariant(m_dataStorage[typeIndex.first].at(typeIndex.second).m_values.at(index.column() - s_ColumnOffset));
}

QVariant LogdataStorage::headerData(int column, Qt::Orientation orientation, int role) const
{
    if ((column == -1) || (role != Qt::DisplayRole) || (orientation == Qt::Vertical))
    {
        return QVariant();
    }
    if (column == 0)
    {
        return QVariant("Index");   // first colum is always the index
    }
    if (column == 1)
    {
        return QVariant("MSG Type");    // second colum is always the message type
    }
    const TypeIndexPair &typeIndex = m_indexToDataRow[m_currentRow];
    if ((column - s_ColumnOffset) >= m_typeStorage[typeIndex.first].m_labels.size())
    {
        return QVariant("");    // this row does not have this column
    }
    return QVariant(m_typeStorage[typeIndex.first].m_labels[column - s_ColumnOffset]);
}

bool LogdataStorage::addDataType(const QString &typeName, const unsigned int typeID, const int typeLength,
                                 const QString &typeFormat, const QStringList &typeLabels, const int timeColum)
{
    // set up colum count - the storage adds s_ColumOffset colums to the data.
    // One for the index and one for the name.
    m_columCount = m_columCount < (typeLabels.size() + s_ColumnOffset) ? typeLabels.size() + s_ColumnOffset : m_columCount;

    // create new type and store it
    dataType NewType(typeName, typeID, typeLength, typeFormat, typeLabels, timeColum);
    m_typeStorage.insert(typeName, NewType);
    // to be able to recreate the order we store the names in a vector.
    m_indexToTypeRow.push_back(typeName);

    return true;
}

bool LogdataStorage::addDataRow(const QString &typeName, const QList<QPair<QString,QVariant> >  &values)
{
    if (!m_typeStorage.count(typeName))  // type exists in type storage?
    {
        QTextStream error(&m_errorText);
        error << "Data of type " << typeName << " cannot be inserted cause its type is unknown.";
        return false;
    }

    dataType &tempType = m_typeStorage[typeName];
    if(values.size() != tempType.m_labels.size())    // Number of elements match type?
    {
        QTextStream error(&m_errorText);
        error << "Number of datafields for type " << typeName << " does not match. Expected:"
              << tempType.m_labels.size() << " got:" << values.size();
        return false;
    }

    // fetch min & max timestamp of all data
    quint64 tempTime = values.at(tempType.m_timeStampIndex).second.toULongLong();
    m_minTimeStamp = m_minTimeStamp > tempTime ? tempTime : m_minTimeStamp;
    m_maxTimeStamp = m_maxTimeStamp < tempTime ? tempTime : m_maxTimeStamp;

    IndexValueRow newRow;
    newRow.m_values.reserve(values.size());
    for(int i = 0; i < values.size(); ++i)
    {
        if(values[i].first != tempType.m_labels[i])  // value name match?
        {
            QTextStream error(&m_errorText);
            error << "Data of type " << typeName << " Value name mismatch. Expected:"
                  << tempType.m_labels[i] << " got:" << values[i].first;
            return false;
        }
        newRow.m_values.push_back(values[i].second);
    }
    // add current global dataindex to row
    newRow.m_index = m_indexToDataRow.size();   // size() will be the index after push_back()
    // add to data storage
    m_dataStorage[typeName].append(newRow);
    // add type name to global dataindex
    m_indexToDataRow.push_back(TypeIndexPair(typeName, m_dataStorage[typeName].size() - 1)); // last index is size() - 1
    // create time to index pair
    TimeStampToIndexPair timeIndex(tempTime, newRow.m_index);
    // and add it to time index
    m_TimeToIndexList.push_back(timeIndex);
    return true;
}

void LogdataStorage::selectedRowChanged(QModelIndex current)
{
    if (!current.isValid() || (m_currentRow == current.row()))
    {
        return;
    }
    m_currentRow = current.row() < m_indexToDataRow.size() ? current.row() : m_indexToDataRow.size() - 1; // size() -1 is last index
    emit headerDataChanged(Qt::Horizontal,0,9);
}

void LogdataStorage::setTimeStamp(const QString &timeStampName, const double &divisor)
{
    m_timeStampName = timeStampName;
    m_timeDivisor = divisor;

    // As this method is called at the End of the parsing we should use the chance to sort the time index by
    // time - just to be sure...
    std::stable_sort(m_TimeToIndexList.begin(), m_TimeToIndexList.end(), TimeStampToIndexPairComparer());
}

double LogdataStorage::getTimeDivisor() const
{
    return m_timeDivisor;
}

QMap<QString, QStringList> LogdataStorage::getFmtValues() const
{
    QMap<QString, QStringList> fmtValueMap; //using a map to get alphabetically sorting

    foreach(const QString &typeName, m_indexToTypeRow)
    {
        if(m_dataStorage.count(typeName))   // only types we have data for
        {
            fmtValueMap.insert(typeName, m_typeStorage[typeName].m_labels);
        }
    }

    return fmtValueMap;
}

QString LogdataStorage::getFmtLine(const QString &name) const
{
    QString fmtLine;
    if(!m_typeStorage.count(name))
    {
        return fmtLine;
    }

    const dataType &tempType = m_typeStorage[name];
    QTextStream fmtStream(&fmtLine);
    fmtStream << "FMT," << tempType.m_ID << "," << tempType.m_length << "," << tempType.m_name << ","
              << tempType.m_format << "," << tempType.m_labels.join(",");

    return  fmtLine;
}

QVector<QPair<double, QVariant> > LogdataStorage::getValues(const QString &parent, const QString &child, const bool useTimeAsIndex) const
{
    QVector<QPair<double, QVariant> > data;

    if(!m_typeStorage.count(parent) || !m_dataStorage.count(parent))
    {
        return data;    // don't have this type or no data for this type
    }

    int valueIndex = m_typeStorage[parent].m_labels.indexOf(child);
    int timeStampIndex = m_typeStorage[parent].m_timeStampIndex;

    if(valueIndex == -1)
    {
        return data;    // don't have this value type
    }

    QPair<double, QVariant> indexValuePair;
    data.reserve(m_dataStorage[parent].size());
    foreach(const IndexValueRow &row, m_dataStorage[parent])
    {
        indexValuePair.first = useTimeAsIndex ? row.m_values.at(timeStampIndex).toDouble() / m_timeDivisor : row.m_index;
        indexValuePair.second = row.m_values.at(valueIndex);
        data.push_back(indexValuePair);
    }

    return data;
}

double LogdataStorage::getMinTimeStamp() const
{
    return static_cast<double>(m_minTimeStamp) / m_timeDivisor;
}

double LogdataStorage::getMaxTimeStamp() const
{
    return static_cast<double>(m_maxTimeStamp) / m_timeDivisor;
}

int LogdataStorage::getNearestIndexForTimestamp(const double timevalue) const
{
    quint64 timeToFind = static_cast<quint64>(m_timeDivisor * timevalue);
    int intervalSize = m_TimeToIndexList.size();
    int intervalStart = 0;
    int middle = 0;

    while (intervalSize > 1)
    {
        middle = intervalStart + intervalSize / 2;
        quint64 tempTime = m_TimeToIndexList[middle].first;
        if (timeToFind > tempTime)
        {
            intervalStart = middle;
        }
        else if (timeToFind == tempTime)
        {
            break;
        }
        intervalSize = intervalSize / 2;
    }
    return m_TimeToIndexList[middle].second;
}

void LogdataStorage::getMessagesOfType(const QString &type, QMap<quint64, MessageBase::Ptr> &indexToMessageMap) const
{
    if(!m_dataStorage.count(type))
    {
        QLOG_DEBUG() << "Graph loaded with no table of type " << type;
        return;
    }

    QList<NameValuePair> nameValueList;

    foreach(const IndexValueRow &row, m_dataStorage[type])
    {
        nameValueList.clear();
        nameValueList.append(NameValuePair("Index", row.m_index));  // Add Data index

        for(int i = 0; i < row.m_values.size(); ++i)
        {
            NameValuePair tempPair(m_typeStorage[type].m_labels.at(i), row.m_values.at(i)); // add names and values
            nameValueList.append(tempPair);
        }
        MessageBase::Ptr msgPtr = MessageFactory::CreateMessageOfType(type, nameValueList, m_timeStampName, m_timeDivisor);
        if(msgPtr)
        {
            indexToMessageMap.insert(static_cast<quint64>(row.m_index), msgPtr);
        }
    }
}

QString LogdataStorage::getError()
{
    return m_errorText;
}
