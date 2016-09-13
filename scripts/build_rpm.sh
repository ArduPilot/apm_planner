#!/bin/bash

if [ -z "$SPEC_FILE" ]
then
    SPEC_FILE="${HOME}/rpmbuild/SPECS/apmplanner2.spec"
fi

RPM_OPTS=""

if [ -n $1 ]
then
    RPM_OPTS="${RPM_OPTS} --target ${1}"
fi

rpmbuild -ba $SPEC_FILE $RPM_OPTS

