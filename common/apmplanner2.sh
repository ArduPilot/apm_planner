#!/bin/sh

# On Intel graphics chipsets, the primary flight display can hog the
# CPU leading to poor system performance. For more details, see:
# https://github.com/ArduPilot/apm_planner/issues/646
#
# To use this wrapper, rename apmplanner2 to apmplanner2.bin and install
# this script as apmplanner2.

export QSG_RENDER_LOOP=threaded
exec ${0}.bin "$@"
