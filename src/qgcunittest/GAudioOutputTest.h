#ifndef GAUDIOOUTPUTTEST_HPP
#define GAUDIOOUTPUTTEST_HPP

#include <QObject>

#include "AutoTest.h"

class GAudioOutputTest : public QObject
{
    Q_OBJECT
public:
    GAudioOutputTest();

private slots:
    void sayNonExistingFileShallNotSegfault();
};

DECLARE_TEST(GAudioOutputTest)
#endif // GAUDIOOUTPUTTEST_HPP
