#include "versioncomparatortest.h"

#include <QtTest/QtTest>

#include "versioncomparator.h"


VersionComparatorTest::VersionComparatorTest()
{
}

void VersionComparatorTest::testHigherMajorNumberIsNewer()
{
    const QString newVersion("v2.3.2-rc1");
    const QString currentVersion("v1.2.4");

    QCOMPARE(VersionComparator::isVersionNewer(newVersion, currentVersion), true);
    QCOMPARE(VersionComparator::isVersionNewer(currentVersion, newVersion), false);
}

void VersionComparatorTest::testEqualMajorNumberIsNotNewer()
{
    const QString newVersion("v1.0.0");
    const QString currentVersion("1.0.0");

    QCOMPARE(VersionComparator::isVersionNewer(newVersion, currentVersion), false);
}

void VersionComparatorTest::testHigherMinorNumberIsNewer()
{
    const QString newVersion("v1.32.24-rc1");
    const QString currentVersion("1.31.4");

    QCOMPARE(VersionComparator::isVersionNewer(newVersion, currentVersion), true);
    QCOMPARE(VersionComparator::isVersionNewer(currentVersion, newVersion), false);
}

void VersionComparatorTest::testEqualMinorNumberIsNotNewer()
{
    const QString newVersion("v1.3.0");
    const QString currentVersion("1.3.0");

    QCOMPARE(VersionComparator::isVersionNewer(newVersion, currentVersion), false);
}

void VersionComparatorTest::testHigherBuildNumberIsNewer()
{
    const QString newVersion("v2.3.24");
    const QString currentVersion("v2.3.20-rc1");

    QCOMPARE(VersionComparator::isVersionNewer(newVersion, currentVersion), true);
    QCOMPARE(VersionComparator::isVersionNewer(currentVersion, newVersion), false);
}

void VersionComparatorTest::testEqualBuildNumberIsNotNewer()
{
    const QString newVersion("2.3.3");
    const QString currentVersion("v2.3.3");

    QCOMPARE(VersionComparator::isVersionNewer(newVersion, currentVersion), false);
}

void VersionComparatorTest::testExistingBuildNumberIsNewerThanNoBuildNumber()
{
    const QString newVersion("2.3.2");
    const QString currentVersion("v2.3");

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

void VersionComparatorTest::testInvalidNewVersionReturnsFalse()
{
    const QString newVersion("abcdef");
    const QString currentVersion("2.3.2-rc1");

    QCOMPARE(VersionComparator::isVersionNewer(newVersion, currentVersion), false);
}

