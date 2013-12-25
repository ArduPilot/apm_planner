#!/bin/sh
# Move and Upload
COMMIT_SHA=$(git rev-parse HEAD)
TARGET_DIR=/Users/Shared/Jenkins/Home/data/APMPlanner/daily
TARGET_SUBDIR=${BUILD_ID:0:10}
WEBSITE_USER=

if [ ! -d ${TARGET_DIR}/${TARGET_SUBDIR} ]; then
  mkdir ${TARGET_DIR}/${TARGET_SUBDIR}/
fi
mv apm_planner2_$BUILD_ID\_osx.dmg ${TARGET_DIR}/${TARGET_SUBDIR}/apm_planner2_${COMMIT_SHA:0:8}_osx.dmg
rsync -avh --password-file=rsyncpass ../../../data/APMPlanner/daily/ ${WEBSITE_USER}@firmware.diydrones.com::APMPlanner/daily/
