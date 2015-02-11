#!/bin/sh
# Move and Upload
COMMIT_SHA=$(git describe --dirty)
TARGET_DIR=/Users/admin/dev_src/APMPlanner
WEBSITE_USER=TeamCityAdmin

if [ ! -d ${TARGET_DIR} ]; then
  mkdir -vp ${TARGET_DIR}/
fi
mv -v apm_planner2_$BUILD_NUMBER\_osx.dmg ${TARGET_DIR}/apm_planner_${COMMIT_SHA}_osx.dmg
rsync -avh --password-file=../../../dev_src/rsyncpass ${TARGET_DIR}/apm_planner_${COMMIT_SHA}_osx.dmg ${WEBSITE_USER}@firmware.diydrones.com::APMPlanner/
