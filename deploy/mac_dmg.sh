#!/bin/sh

if [ -f "release/apmplanner2.dmg" ] ; then
    rm -rf "release/apmplanner2.dmg"
fi

appdmg deploy/assets/config.json  release/apmplanner2.dmg
