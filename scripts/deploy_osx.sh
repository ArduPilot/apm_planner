#!/bin/sh
echo "Deploy Step"
trap 'echo "error"; do_cleanup failed; exit' ERR
trap 'echo "received signal to stop"; do_cleanup interrupted; exit' SIGQUIT SIGTERM SIGINT

function do_cleanup {
    hdiutil eject /Volumes/APM\ Planner\ 2.0/
    rm apm_planner2_target.sparseimage
    rm -rf /release
}

# Locate macdeployqt from the Qt6 install (preferred), falling back to PATH.
# Override either by exporting MACDEPLOYQT or APP_BUNDLE.
MACDEPLOYQT=${MACDEPLOYQT:-"$(brew --prefix qt 2>/dev/null)/bin/macdeployqt"}
if [ ! -x "$MACDEPLOYQT" ]; then
    MACDEPLOYQT=$(command -v macdeployqt)
fi
# CMake builds the bundle in the build dir; the legacy qmake build used release/.
APP_BUNDLE=${APP_BUNDLE:-"apmplanner2.app"}
[ -d "$APP_BUNDLE" ] || APP_BUNDLE="release/apmplanner2.app"

"$MACDEPLOYQT" "$APP_BUNDLE" -qmldir=../qml
cp ../../../dev_src/apm_planner2_base.sparseimage apm_planner2_target.sparseimage
hdiutil attach apm_planner2_target.sparseimage
wait
cp -pRP "$APP_BUNDLE" /Volumes/APM\ Planner\ 2.0/APM\ Planner\ 2.0.app
hdiutil eject /Volumes/APM\ Planner\ 2.0/
hdiutil convert apm_planner2_target.sparseimage -format UDBZ -o apm_planner2_$BUILD_NUMBER\_osx.dmg
echo "Deploy Complete of " apm_planner2_$BUILD_NUMBER\_osx.dmg
echo $(date)
