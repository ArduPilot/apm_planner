#!/bin/bash
# Move and Upload
COMMIT_SHA=$(git describe --dirty)
TARGET_DIR=/var/www/wordpress/APMPlanner2/daily
TARGET_SUBDIR=${BUILD_ID:0:10}

if [ ! -d ${TARGET_DIR}/${TARGET_SUBDIR} ]; then
  mkdir ${TARGET_DIR}/${TARGET_SUBDIR}/
fi

mv apm_planner_${BUILD_TAG}_wheezy.i386.deb ${TARGET_DIR}/${TARGET_SUBDIR}/
mv apm_planner_${BUILD_TAG}_wheezy.amd64.deb ${TARGET_DIR}/${TARGET_SUBDIR}/
mv apm_planner_${BUILD_TAG}_raring.i386.deb ${TARGET_DIR}/${TARGET_SUBDIR}/
mv apm_planner_${BUILD_TAG}_raring.amd64.deb ${TARGET_DIR}/${TARGET_SUBDIR}/

#This will eventually contain my rsync line...
rsync -avh --password-file=${RSYNC_PASSFILE} ${TARGET_DIR}/${TARGET_SUBDIR} ${WEBSITE_USER}@firmware.diydrones.com::APMPlanner/daily/
