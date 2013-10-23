#!/bin/sh
echo "Deploy Step"
trap 'echo "error"; do_cleanup failed; exit' ERR
trap 'echo "received signal to stop"; do_cleanup interrupted; exit' SIGQUIT SIGTERM SIGINT

function do_cleanup {
    hdiutil eject /Volumes/APM\ Planner\ 2.0\ \(Alpha\)/
    rm apm_planner2_target.sparseimage
}

macdeployqt apmplanner2.app/
cp apm_planner2_base.sparseimage apm_planner2_target.sparseimage
hdiutil attach apm_planner2_target.sparseimage
wait
cp -pRP apmplanner2.app /Volumes/APM\ Planner\ 2.0\ \(Alpha\)/APM\ Planner\ 2.0
hdiutil eject /Volumes/APM\ Planner\ 2.0\ \(Alpha\)/
hdiutil convert apm_planner2_target.sparseimage -format UDBZ -o apm_planner2_$BUILD_ID.dmg
echo "Deploy Complete"
