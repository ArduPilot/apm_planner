#!/bin/sh
#
# Sends download notification to Gitter Custom webhook
#
COMMIT_SHA=$(git describe --dirty)
TARGET_SUBDIR=daily/$(date "+%%Y-%%m-%%d")
VERSION=COMMIT_SHA
MESSAGE="APM Planner ${VERSION} is available to [download](http://firmware.diydrones.com/Tools/APMPlanner/${TARGET_SUBDIR}/)"

GITTER_WEBHOOK_DEV=https://webhooks.gitter.im/e/<blah>
GITTER_WEBHOOK_MAIN=https://webhooks.gitter.im/e/<blah>

curl -d message=${MESSAGE} $GITTER_WEBHOOK_DEV
# curl -d message=${MESSAGE} $GITTER_WEBHOOK_MAIN
