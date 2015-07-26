#!/bin/bash

if [ -z "$SPEC_FILE" ]
then
    SPEC_FILE="${HOME}/rpmbuild/SPECS/apmplanner2.spec"
fi

rpmbuild -ba $SPEC_FILE --target=i686
rpmbuild -ba $SPEC_FILE --target=x86_64
