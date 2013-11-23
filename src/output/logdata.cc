
#include <QStringList>

#include "logdata.h"

void DataLine::readFields(FormatLine& format, QString &line) {
    QStringList parts = line.split(QRegExp(","), QString::KeepEmptyParts);

    const int size = parts.length();
    if(size == (format.fields.length() + 1)) {
        for(int i = 1; i < size; ++i) {
            QString key = format.fields[i-1];
            QString value = parts[i].trimmed();
            values.insert(key, value);
        }
    }
}

FormatLine FormatLine::from(QString &line) {
    QStringList parts = line.split(QRegExp(","), QString::KeepEmptyParts);

    FormatLine f;

    int i = 1;
    f.type = parts[i++].trimmed().toUInt();
    f.length = parts[i++].trimmed().toUInt();
    f.name = parts[i++].trimmed();
    f.format = parts[i++].trimmed();

    while(i < parts.length()) {
        f.fields.append(parts[i++].trimmed());
    }

    return f;
}
