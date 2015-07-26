#include "versioncomparatortest.h"

#include <QtTest/QtTest>

#include "versioncomparator.h"


VersionComparatorTest::VersionComparatorTest()
{
}

void VersionComparatorTest::testHigherMajorNumberIsNewer()
{
    const QString newVersion("2.3.2-rc1");
    const QString currentVersion("1.2.4");

    QCOMPARE(VersionComparator::isVersionNewer(newVersion, currentVersion), true);
    QCOMPARE(VersionComparator::isVersionNewer(currentVersion, newVersion), false);
}

void VersionComparatorTest::testEqualMajorNumberIsNotNewer()
{
    const QString newVersion("1.0.0");
    const QString currentVersion("1.0.0");

    QCOMPARE(VersionComparator::isVersionNewer(newVersion, currentVersion), false);
}

void VersionComparatorTest::testHigherMinorNumberIsNewer()
{
    const QString newVersion("1.3.2-rc1");
    const QString currentVersion("1.2.4");

    QCOMPARE(VersionComparator::isVersionNewer(newVersion, currentVersion), true);
    QCOMPARE(VersionComparator::isVersionNewer(currentVersion, newVersion), false);
}

void VersionComparatorTest::testEqualMinorNumberIsNotNewer()
{
    const QString newVersion("1.3.0");
    const QString currentVersion("1.3.0");

    QCOMPARE(VersionComparator::isVersionNewer(newVersion, currentVersion), false);
}

void VersionComparatorTest::testHigherBuildNumberIsNewer()
{
    const QString newVersion("2.3.4");
    const QString currentVersion("2.3.2-rc1");

    QCOMPARE(VersionComparator::isVersionNewer(newVersion, currentVersion), true);
    QCOMPARE(VersionComparator::isVersionNewer(currentVersion, newVersion), false);
}

void VersionComparatorTest::testEqualBuildNumberIsNotNewer()
{
    const QString newVersion("2.3.3");
    const QString currentVersion("2.3.3");

    QCOMPARE(VersionComparator::isVersionNewer(newVersion, currentVersion), false);
}

void VersionComparatorTest::testExistingBuildNumberIsNewerThanNoBuildNumber()
{
    const QString newVersion("2.3.2");
    const QString currentVersion("2.3");

    QCOMPARE(VersionComparator::isVersionNewer(newVersion, currentVersion), true);
    QCOMPARE(VersionComparator::isVersionNewer(currentVersion, newVersion), false);
}

void VersionComparatorTest::testNoReleaseCandidateIsNewerThanExistingReleaseCandidate()
{
    const QString newVersion("2.3.2");
    const QString currentVersion("2.3.2-rc1");

    QCOMPARE(VersionComparator::isVersionNewer(newVersion, currentVersion), true);
    QCOMPARE(VersionComparator::isVersionNewer(currentVersion, newVersion), false);
}

void VersionComparatorTest::testHigherReleaseCandidateIsNewer()
{
    const QString newVersion("2.3.2-rc3");
    const QString currentVersion("2.3.2-rc1");

    QCOMPARE(VersionComparator::isVersionNewer(newVersion, currentVersion), true);
    QCOMPARE(VersionComparator::isVersionNewer(currentVersion, newVersion), false);
}

