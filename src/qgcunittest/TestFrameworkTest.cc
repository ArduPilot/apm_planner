#include <QtTest/QtTest>

#include "TestFrameworkTest.h"

//This function is called before every test
void TestFrameworkTest::init()
{
}

//this function is called after every test
void TestFrameworkTest::cleanup()
{
}

void TestFrameworkTest::testStringComparison()
{
    QString test("test");
    QVERIFY(test == "test");
}

void TestFrameworkTest::testIntComparison()
{
    int x = 5;
    QVERIFY(x == 5);
}
