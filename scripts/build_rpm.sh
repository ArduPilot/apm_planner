#!/bin/bash

RPMBUILD_ROOT="${HOME}/rpmbuild"

rpmbuild -ba "${RPMBUILD_ROOT}/SPECS/apmplanner2.spec"
