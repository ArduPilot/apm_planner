#!/bin/sh
echo "Deploy Step"
trap 'echo "error"; do_cleanup failed; exit' ERR
trap 'echo "received signal to stop"; do_cleanup interrupted; exit' SIGQUIT SIGTERM SIGINT

function do_cleanup {
    hdiutil eject /Volumes/APM\ Planner\ 2.0/
    rm apm_planner2_target.sparseimage
    rm -rf /release
}

/Developer/Qt5.2.1/5.2.1/clang_64/bin/macdeployqt release/apmplanner2.app/
cp ../../../dev_src/apm_planner2_base.sparseimage apm_planner2_target.sparseimage
hdiutil attach apm_planner2_target.sparseimage
wait
cp -pRP release/apmplanner2.app /Volumes/APM\ Planner\ 2.0/APM\ Planner\ 2.0.app
hdiutil eject /Volumes/APM\ Planner\ 2.0/
hdiutil convert apm_planner2_target.sparseimage -format UDBZ -o apm_planner2_$BUILD_NUMBER\_osx.dmg
echo "Deploy Complete of " apm_planner2_$BUILD_NUMBER\_osx.dmg
echo $(date)
