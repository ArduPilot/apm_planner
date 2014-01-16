#!/bin/sh
# Cleanup previous files
echo "Cleanup old app and intermediary disk image files"
if [ -d apmplanner2.app ]
then
    echo "remove old app package"
    rm -rf apmplanner2.app/
fi

if [ -e apm_planner2_target.sparseimage ]
then
    echo "Remove apm_planner2_target.sparseimage"
    rm apm_planner2_target.sparseimage
fi
