#!/bin/bash
# Move and Upload
COMMIT_SHA=$(git rev-parse HEAD)
TARGET_DIR=/var/www/wordpress/APMPlanner2/daily
TARGET_SUBDIR=${BUILD_ID:0:10}

if [ ! -d ${TARGET_DIR}/${TARGET_SUBDIR} ]; then
  mkdir ${TARGET_DIR}/${TARGET_SUBDIR}/
fi

mv apmplanner2-installer-win32.exe ${TARGET_DIR}/${TARGET_SUBDIR}/apm_planner2_${COMMIT_SHA:0:8}_win.exe

#This will eventually contain my rsync line...
rsync -avh --password-file=${RSYNC_PASSFILE} ${TARGET_DIR}/${TARGET_SUBDIR} ${WEBSITE_USER}@firmware.diydrones.com::APMPlanner/daily/
