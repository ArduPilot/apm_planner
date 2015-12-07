#include "GAudioOutputTest.h"

#include <QtTest/QtTest>

#include <QApplication>
#include <QSettings>

#include "configuration.h"
#include "globalobject.h"
#include "GAudioOutput.h"

GAudioOutputTest::GAudioOutputTest()
{
    QSettings::setDefaultFormat(QSettings::IniFormat);

    int argc = 0;
    char *args[] = {};

    QApplication app(argc, args);

    app.setOrganizationName("diydrones");
    app.setApplicationName(QString(QGC_APPLICATION_NAME) + "_test");

    GlobalObject::sharedInstance()->setAppDataDirectory("/tmp");
    GlobalObject::sharedInstance()->saveSettings();
}


void GAudioOutputTest::sayNonExistingFileShallNotSegfault()
{
    QDir dir("/tmp/tmp_audio");
    dir.removeRecursively();

    QCOMPARE(true, GAudioOutput::instance()->say("Non existing file"));
}
