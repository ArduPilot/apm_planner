#!/bin/bash

LATEST_RELEASE="2.0"

PROJECT_NAME="apmplanner2"
REPO_URL="ssh://git@github.com/ardupilot/apm_planner.git"

RPMBUILD_ROOT="${HOME}/rpmbuild"
PROJECT_FILE_NAME="apm_planner.pro"

if [[ -f "../${PROJECT_FILE_NAME}" ]]
then
	WORK_PATH=".."
elif [[ -f "./${PROJECT_FILE_NAME}" ]]
then
	WORK_PATH="."
else
	WORK_PATH="apm_planner"
fi
WORK_PATH=$(realpath "$WORK_PATH")

if [[ -d "$WORK_PATH" && -d "${WORK_PATH}/.git" ]]
then
	git -C "$WORK_PATH" pull
else
	git clone "$REPO_URL" "$WORK_PATH"
fi

read -a V <<< $(git -C "$WORK_PATH" describe | sed "s/\-/ /")
VERSION="${V[0]}"
RELEASE=$(sed "s/\-/_/" <<< ${V[1]})

TARBALL_PATH="${RPMBUILD_ROOT}/SOURCES/${PROJECT_NAME}-${VERSION}.tar.gz"
rm -f "$TARBALL_PATH"
echo -n "Packing source tarball $TARBALL_PATH ... "
WPREGEX=$(sed "s/^\///" <<< $WORK_PATH)
tar --exclude-vcs --exclude=Drivers --exclude=avrdude --exclude=uploader --xform=s%^${WPREGEX}%${PROJECT_NAME}-${VERSION}/% -z -c -f ${TARBALL_PATH} ${WORK_PATH}
echo "done"

OLD_SPEC="${WORK_PATH}/redhat/apmplanner2.spec";
NEW_SPEC="${RPMBUILD_ROOT}/SPECS/${PROJECT_NAME}.spec";

echo -n "Writing ${PROJECT_NAME}.spec ... "
sed -e "s/\(Version:\s\+\).\+/\1${VERSION}/; s/\(Release:\s\+\).\+/\1${RELEASE}/" < $OLD_SPEC > $NEW_SPEC
echo "done"

