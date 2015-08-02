#ifndef VERSIONCOMPARATORTEST_H
#define VERSIONCOMPARATORTEST_H

#include <QObject>

#include "AutoTest.h"

class VersionComparatorTest : public QObject
{
    Q_OBJECT
public:
    VersionComparatorTest();

private slots:
    void testHigherMajorNumberIsNewer();

    void testHigherMajorNumberWithoutBuildNumberIsNewer();

    void testEqualMajorNumberIsNotNewer();

    void testHigherMinorNumberIsNewer();

    void testHigherMinorNumberWithoutBuildNumberIsHigher();

    void testEqualMinorNumberIsNotNewer();

    void testHigherBuildNumberIsNewer();

    void testEqualBuildNumberIsNotNewer();

    void testExistingBuildNumberIsNewerThanNoBuildNumber();

    void testNoReleaseCandidateIsNewerThanExistingReleaseCandidate();

    void testHigherReleaseCandidateIsNewer();

    void testInvalidNewVersionReturnsFalse();
};

DECLARE_TEST(VersionComparatorTest)
#endif // VERSIONCOMPARATORTEST_H
