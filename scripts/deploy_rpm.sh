#!/bin/bash

################################################################################
#
# Usage:
#
# export REPO_BASE_PATH=/var/www/html/diydrones
# export RPMBUILD_ROOT=/home/user/rpmbuild
# ./deploy_rpm.sh
#
################################################################################
#
# Environement vars
#
# REPO_BASE_PATH - path to the root of Yum repository (default to /var/www/html/diydrones)
# RPMBUILD_ROOT  - path to rpmbuild tree (default to ~/rpmbuild)
#
################################################################################
#
# Yum repository tree (repodata dirs are omitted)
#
# /var/www/html/diydrones
# ├── i686
# │   ├── apmplanner2-2.0.16-rc2.13.g1e89f9f.fc20.i686.rpm
# │   └── debug
# │       └── apmplanner2-debuginfo-2.0.16-rc2.13.g1e89f9f.fc20.i686.rpm
# ├── x86_64
# │   ├── apmplanner2-2.0.16-rc2.13.g1e89f9f.fc20.x86_64.rpm
# │   └── debug
# │       └── apmplanner2-debuginfo-2.0.16-rc2.13.g1e89f9f.fc20.x86_64.rpm
# └── SPRMS
#     └── apmplanner2-2.0.16-rc2.13.g1e89f9f.fc20.src.rpm
#
################################################################################
#
# Yum .repo file see in ../redhat/diydrones.repo
#
################################################################################

if [ -z "$REPO_BASE_PATH" ]
then
    REPO_BASE_PATH="/var/www/html/diydrones"
fi

if [ -z "$RPMBUILD_ROOT" ]
then
    RPMBUILD_ROOT="${HOME}/rpmbuild"
fi

CHECKOUT=($(git describe | sed -r "s/(([0-9]+\.)+[0-9]+)-(.+)/\1 \3/g"))
VERSION=${CHECKOUT[0]}
RELEASE=$(echo ${CHECKOUT[1]} | sed -r "s/-/./g")
DIST=$(rpmbuild --showrc | grep  ': dist' | sed -r 's/^.+dist\s+//')

for ARCH in i686 x86_64
do
    # Deploy binary RPMs
    RPM_REPO="${REPO_BASE_PATH}/${ARCH}"
    if [ ! -d $RPM_REPO ]
    then
        mkdir -p $RPM_REPO
    fi
    mv ${RPMBUILD_ROOT}/RPMS/${ARCH}/apmplanner2-${VERSION}-${RELEASE}${DIST}.${ARCH}.rpm $RPM_REPO
    createrepo_c --update --excludes=*-debuginfo-* ${RPM_REPO}

    # Deploy debuginfo RPMs
    DEBUG_REPO="${RPM_REPO}/debug"
    if [ ! -d $DEBUG_REPO ]
    then
        mkdir -p $DEBUG_REPO
    fi
    mv ${RPMBUILD_ROOT}/RPMS/${ARCH}/apmplanner2-debuginfo-${VERSION}-${RELEASE}${DIST}.${ARCH}.rpm $DEBUG_REPO
    createrepo_c --update $DEBUG_REPO
done

# Deploy source RPMs
SOURCE_REPO="${REPO_BASE_PATH}/SPRMS"
if [ ! -d $SOURCE_REPO ]
then
    mkdir -p $SOURCE_REPO
fi
mv ${RPMBUILD_ROOT}/SRPMS/apmplanner2-${VERSION}-${RELEASE}${DIST}.src.rpm $SOURCE_REPO
createrepo_c --update $SOURCE_REPO

