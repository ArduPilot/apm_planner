#ifndef TESTFRAMEWORKTEST_H
#define TESTFRAMEWORKTEST_H

#include <QObject>

#include "AutoTest.h"

class TestFrameworkTest : public QObject
{
    Q_OBJECT

private slots:
  void init();
  void cleanup();
  
private slots:
  void testStringComparison();

  void testIntComparison();
};

DECLARE_TEST(TestFrameworkTest)
#endif // TESTFRAMEWORKTEST_H
