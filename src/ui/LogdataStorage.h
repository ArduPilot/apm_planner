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
 * @file LogdataStorage.h
 * @author Arne Wischmann <wischmann-a@gmx.de>
 * @date 16 Nov 2016
 * @brief File providing header for the data storage for logfiles
 */

#ifndef LOGDATASTORAGE_H
#define LOGDATASTORAGE_H

#include <QObject>
#include <QAbstractTableModel>
#include <ArduPilotMegaMAV.h>

/**
 * @brief The LogdataStorage class is used to store the data parsed from logfiles.
 *        Besides that it implements the QAbstractTableModel to provide a well known
 *        interface. Additionally it brings some helper an convenience functions.
 *
 *        To add some data first a dataType (by calling addDataType(...)) has to be added.
 *        After that data values for this type can be added. They must respect the format
 *        of the added type.
 *
 */
class LogdataStorage : public QAbstractTableModel
{
    Q_OBJECT
public:

    typedef QPair<quint64, int> TimeStampToIndexPair;    /// Type for holding a time stamp and its corresponding index

    /**
     * @brief Ptr - shared pointer type for this class
     */
    typedef QSharedPointer<LogdataStorage> Ptr;

    /**
     * @brief LogdataStorage - CTOR
     */
    explicit LogdataStorage();

    /**
     * @brief ~LogdataStorage - DTOR
     */
    virtual ~LogdataStorage();

    /**
     * @see help of QAbstractTableModel::rowCount
     */
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

    /**
     * @see help of QAbstractTableModel::columnCount
     */
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

    /**
     * @see help of QAbstractTableModel::data
     */
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    /**
     * @see help of QAbstractTableModel::headerData
     */
    virtual QVariant headerData(int column, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    /**
     * @brief addDataType adds a new data type to the model. The type is used to validate the data
     *        which is added with the addDataRow() method
     * @param typeName - name of the type to ba added
     * @param typeID - ID of the type to ba added
     * @param typeLength - Length of the type to ba added (bytes)
     * @param typeFormat - format string like "QbbI"
     * @param typeLabels - List of labels for each value in message (colums).
     * @param timeColum - column index of the time stamp field
     *
     * @return - true success, false otherwise (data was not added)
     */
    virtual bool addDataType(const QString &typeName, const unsigned int typeID, const int typeLength,
                             const QString &typeFormat, const QStringList &typeLabels, const int timeColum);

    /**
     * @brief addDataRow adds a data row - a list of pairs of string and value - to the data storage.
     *        The type of the data must be declared with addDataType(...) first. If the method
     *        returns false the reason can be read using the getError() method.
     * @param typeName - Type name of the data
     * @param values - list of pairs containing name and value of a measurement
     * @return - true success, false otherwise (data was not added)
     */
    virtual bool addDataRow(const QString &typeName, const QList<QPair<QString,QVariant> >  &values);

    /**
     * @brief selectedRowChanged must be called if the selected row in the datamodel changes.
     *        It is needed to emit the right header data for the selected row.
     * @param current - QModelIndex holding the current row.
     */
    virtual void selectedRowChanged(QModelIndex current);

    /**
     * @brief setTimeStamp - sets the time stamp type of the datamodel. It will be used
     *        for scaling the time stamps to seconds. This method MUST be called at the END
     *        of the parsing process as it does some finalizings on the datamodel.
     *
     * @param timeStampName - name of the timestamp field. All types MUST have the same.
     * @param divisor - divisor used to scale the Time stamps to seconds
     */
    virtual void setTimeStamp(const QString &timeStampName, const double &divisor);

    /**
     * @brief getTimeDivisor - Getter for the divisor for time stamps
     * @return - the current divisor
     */
    virtual double getTimeDivisor() const;

    /**
     * @brief getFmtValues delivers a map of all data types with a list of the names of
     *        the values. For example: (IMU,[TimeMs, GyrX, GyrY...])
     * @return - the map containing the data. Only types which have data are listed.
     */
    virtual QMap<QString, QStringList> getFmtValues() const;

    // TODO This must be removed as this function does NOT belong to the storage. It should be moved to the
    // logfile exporter class as soon as this class exists!!! For now just for convenience!
    virtual QString getFmtLine(const QString &name) const;

    /**
     * @brief getValues - delivers a vector of measurements of one type. The pair contains an index on first
     *        and the value on second. The index can be the normal index or a time stamp.
     * @param parent - The name of the type conatinig the measurement like "IMU"
     * @param child - The name of the column like "GyrX"
     * @param useTimeAsIndex - true - use time in index
     * @return The data Vector
     */
    virtual QVector<QPair<double, QVariant> > getValues(const QString &parent, const QString &child, const bool useTimeAsIndex) const;

    /**
     * @brief getMinTimeStamp - getter for the smallest timestamp in log
     * @return - the smallest timestamp scaled to seconds
     */
    virtual double getMinTimeStamp() const;

    /**
     * @brief getMaxTimeStamp - getter for the biggest timestamp in log
     * @return - the biggest timestamp scaled to seconds
     * @return
     */
    virtual double getMaxTimeStamp() const;

    /**
     * @brief getNearestIndexForTimestamp delivers the row index which has the
     *        smallest deviation in its timeStamp to the delivered timeValue.
     *
     * @param timevalue - The timeStamp to search for
     * @return The index with the best timestamp match.
     */
    virtual int getNearestIndexForTimestamp(const double timevalue) const;

    /**
     * @brief getMessagesOfType fetches the special messages (ModeMessage, ErrorMessage,
     *        EventMessage, MsgMessage) from the datamodel. These messages need special handling
     *        as they are events.
     *
     * @param type - Type the message to be fetched. like ModeMessage::TypeName or ErrorMessage::TypeName
     * @param indexToMessageMap - A map to store the results. The Map maintains the order.
     */
    virtual void getMessagesOfType(const QString &type, QMap<quint64, MessageBase::Ptr> &indexToMessageMap) const;

    /**
     * @brief getError delivers the last error detected by the datamodel. This method should be called
     *        if the addDataRow(...) returns false. In that case the reason could be read here.
     *
     * @return String conatining the reason for the error.
     */
    virtual QString getError();

private:

    static const int s_ColumnOffset = 2;                 /// Offset for colums cause model adds index and name column

    typedef QPair<QString, QVariant> NameValuePair;     /// Type holding lable string and its value
    typedef QPair<QString, int> TypeIndexPair;          /// Type holding name and index
    typedef QVector<QVariant> ValueRow;                 /// Type holding one data line of a specific type

    /**
     * @brief The IndexValueRow struct adds a dedicated index field to the
     *        ValueRow.
     */
    struct IndexValueRow
    {
        int m_index;        /// the global index of this row
        ValueRow m_values;  /// The value row

        IndexValueRow() : m_index(0)
        {}
    };

    typedef QVector<IndexValueRow> ValueTable;            /// Type holding all data rows of a specific type

    /**
     * @brief The dataType struct holds all data describing a datatype
     */
    struct dataType
    {
        QString m_name;         /// Name of the type
        unsigned int m_ID;      /// ID of the type
        int m_length;           /// Length in bytes
        QString m_format;       /// format string like "QBB"
        QStringList m_labels;   /// Lable (name) of each column
        int m_timeStampIndex;   /// Index of the time stamp field - for faster access

        dataType() : m_ID(0xFF), m_length(0), m_timeStampIndex(0)
        {}

        dataType(const QString &name, const unsigned int ID, const int length,
                 const QString &format, const QStringList &labels, const int timeColum) :
            m_name(name), m_ID(ID), m_length(length),
            m_format(format), m_labels(labels), m_timeStampIndex(timeColum)
        {}

    };

    int m_columCount;           /// Holds the maximum column count of all rows
    int m_currentRow;           /// The current selected row in table

    QString m_timeStampName;    /// Holds the name of the time stamp
    double  m_timeDivisor;      /// Scaling for timestamp for creating second scaling
    quint64 m_minTimeStamp;     /// the min time stamp in data
    quint64 m_maxTimeStamp;     /// the max time stamp in data

    QVector<TimeStampToIndexPair> m_TimeToIndexList;    /// List holding pairs of time stamp and table row index

    QHash<QString, dataType> m_typeStorage;     /// Holds all known types
    QVector<QString>         m_indexToTypeRow;  /// Holds the Type name in the order they were added

    QHash<QString, ValueTable> m_dataStorage;    /// Holds the complete data
    QVector<TypeIndexPair>     m_indexToDataRow; /// The global index pointing to the row

    QString m_errorText;                         /// Used to store current error

};

#endif // LOGDATASTORAGE_H
