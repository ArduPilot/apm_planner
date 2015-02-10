#!/bin/sh
# Move and Upload
COMMIT_SHA=$(git describe --dirty)
TARGET_DIR=/Users/admin/dev_src/APMPlanner/daily
TARGET_SUBDIR=$(date "+%%Y-%%m-%%d")
WEBSITE_USER=TeamCityAdmin

if [ ! -d ${TARGET_DIR}/${TARGET_SUBDIR} ]; then
  mkdir -vp ${TARGET_DIR}/${TARGET_SUBDIR}/
fi
mv -v apm_planner2_$BUILD_NUMBER\_osx.dmg ${TARGET_DIR}/${TARGET_SUBDIR}/apm_planner_${COMMIT_SHA}_osx.dmg
echo rsync -avh --password-file=../../../dev_src/rsyncpass ${TARGET_DIR}/${TARGET_SUBDIR}/ ${WEBSITE_USER}@firmware.diydrones.com::APMPlanner/daily/${TARGET_SUBDIR}/
