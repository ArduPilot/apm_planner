#!/bin/sh
COMMIT_SHA=$(git describe --dirty)
TARGET_DIR=/Users/admin/dev_src/APMPlanner
WEBSITE_USER=TeamCityAdmin

mv apm_planner2_$BUILD_ID\_osx.dmg ${TARGET_DIR}/apm_planner_${COMMIT_SHA}_osx.dmg
cp ${TARGET_DIR}/apm_planner_${COMMIT_SHA}_osx.dmg ${TARGET_DIR}/apm_planner2_latest_osx.dmg
rsync -avh --password-file=../../../data/rsyncpass ${TARGET_DIR}/apm_planner_${COMMIT_SHA}_osx.dmg  ${WEBSITE_USER}@firmware.diydrones.com::APMPlanner/
rsync -avh --password-file=../../../data/rsyncpass ${TARGET_DIR}/apm_planner2_latest_osx.dmg  ${WEBSITE_USER}@firmware.diydrones.com::APMPlanner/
