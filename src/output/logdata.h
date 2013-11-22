#ifndef LOGDATA_H
#define LOGDATA_H

#include <QHash>

/**
 * @brief The FormatLine struct describes an FMT line appearing in a dataflash log.
 *  Field names, type and length are retrieved from the line in case something
 * needs it.
 */
struct FormatLine {
    unsigned int type;
    unsigned int length;
    QString format;
    QString name;
    QStringList fields;

    bool hasData() {
        return (type > 0 && !name.isEmpty());
    }

    /**
     * @brief from initializes a FormatLine from a line of text.
     */
    static FormatLine from(QString& line);
};

/**
 * @brief The DataLine struct contains a hashtable of the values found in a line.
 * Before using it, you need a FormatLine for the type of line you're going to
 * read. Given that, call the readFields(FormatLine&, QString&) function to populate
 * the hashtable with the values in the line.
 */
struct DataLine {
    QHash<QString, QString> values;

    virtual bool hasData() = 0;

protected:
    void readFields(FormatLine& format, QString& line);
};


#endif // LOGDATA_H
