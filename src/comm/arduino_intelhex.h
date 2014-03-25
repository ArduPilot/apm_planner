#ifndef ARDUINO_INTELHEX_H
#define ARDUINO_INTELHEX_H

#include <QByteArray>
#include <QtWidgets>
#include <QTextStream>

class ArduinoIntelHex : public QByteArray
{
public:
    void loadIntelHex(QString filename);

};

#endif // ARDUINO_INTELHEX_H
