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

    void testEqualMajorNumberIsNotNewer();

    void testHigherMinorNumberIsNewer();

    void testEqualMinorNumberIsNotNewer();

    void testHigherBuildNumberIsNewer();

    void testEqualBuildNumberIsNotNewer();

    void testExistingBuildNumberIsNewerThanNoBuildNumber();

    void testNoReleaseCandidateIsNewerThanExistingReleaseCandidate();

    void testHigherReleaseCandidateIsNewer();


};

DECLARE_TEST(VersionComparatorTest)
#endif // VERSIONCOMPARATORTEST_H
