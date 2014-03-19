#!/bin/sh
#!/bin/sh
COMMIT_SHA=$(git describe --dirty)
TARGET_DIR=/Users/Shared/Jenkins/Home/data/APMPlanner
WEBSITE_USER=

mv apm_planner2_$BUILD_ID\_osx.dmg ${TARGET_DIR}/apm_planner_${COMMIT_SHA}_osx.dmg 
cp ${TARGET_DIR}/apm_planner_${COMMIT_SHA}_osx.dmg ${TARGET_DIR}/apm_planner2_latest_osx.dmg
rsync -avh --password-file=rsyncpass ${TARGET_DIR} ${WEBSITE_USER}@firmware.diydrones.com::APMPlanner/
