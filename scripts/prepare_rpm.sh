#!/bin/sh

LATEST_RELEASE=2.0

PROJECT_NAME=apmplanner2
REPO_URL=https://github.com/diydrones/apm_planner

RPMBUILD_ROOT="${HOME}/rpmbuild"

if [[ -f ../qgroundcontrol.pro ]]
then
    WORK_PATH=..
elif [[ -f qgroundcontrol.pro ]]
then
    WORK_PATH=.
else
    WORK_PATH=apm_planner
fi
WORK_PATH=$(realpath ${WORK_PATH})

if [[ -d $WORK_PATH && -d "${WORK_PATH}/.git" ]]
then
    echo "Repo found in ${WORK_PATH}. Updating ... "
    git -C ${WORK_PATH} pull
else
    echo "Repo not found. Cloning ${REPO_URL} into ${WORK_PATH} ... "
    git clone ${REPO_URL} ${WORK_PATH}
fi
echo "done"

CHECKOUT=$(git -C ${WORK_PATH} describe)
VERSION=$(echo "$CHECKOUT" | grep -Po ^[^-]\+)
RELEASE=$(echo "$CHECKOUT" | grep -Po \(?\<=-\).\+$ | sed s/-/./g ) 

TARBALL_PATH="${RPMBUILD_ROOT}/SOURCES/${PROJECT_NAME}-${VERSION}.tar.gz"
rm -f $TARBALL_PATH;
echo "Packing source tarball ${TARBALL_PATH} ... "
WORK_PATH_REGEX=$( echo "${WORK_PATH}" | sed s/^\\\/// )
tar --exclude-vcs --exclude=Drivers --exclude=avrdude --exclude=uploader --xform=s%^${WORK_PATH_REGEX}%${PROJECT_NAME}-${VERSION}/% -z -c -f ${TARBALL_PATH} ${WORK_PATH} ${PATCHES}
echo "done";

OLD_SPEC="${WORK_PATH}/redhat/apmplanner2.spec"
NEW_SPEC="${RPMBUILD_ROOT}/SPECS/${PROJECT_NAME}.spec"

echo -n "Writing ${PROJECT_NAME}.spec file ... "
sed -e "s/^\(Version:\s\+\).*$/\1${VERSION}/; s/^\(Release:\s\+\).*$/\1${RELEASE}%{?dist}/" < ${OLD_SPEC} > ${NEW_SPEC}
echo "done"

echo -n "Copying diydrones.repo ... "
REPO_FILE="${WORK_PATH}/redhat/diydrones.repo"
cp "${REPO_FILE}" "${RPMBUILD_ROOT}/SOURCES"
echo "done"

echo -n "Copying diydrones-repo.spec ... "
OLD_SPEC="${WORK_PATH}/redhat/diydrones-repo.spec"
cp "${OLD_SPEC}" "${RPMBUILD_ROOT}/SPECS"
echo "done"
