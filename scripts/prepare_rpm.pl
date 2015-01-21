#!/usr/bin/perl

$LATEST_RELEASE = "2.0";

$PROJECT_NAME = "apmplanner2";
$REPO_URL = "https://github.com/diydrones/apm_planner";

$RPMBUILD_ROOT = $ENV{HOME} . "/rpmbuild";

if (-f "../qgroundcontrol.pro") {
    $WORK_PATH = "..";
} elsif (-f "qgroundcontrol.pro") {
    $WORK_PATH = ".";
} else {
    $WORK_PATH = $PROJECT_NAME;
}

if (-d $WORK_PATH && -d "${WORK_PATH}/.git") {
    system "git -C ${WORK_PATH} pull"
} else {
    system "git clone ${REPO_URL} ${WORK_PATH}";
}

$CHECKOUT = `git -C ${WORK_PATH} describe`;
$CHECKOUT =~ s/\n.*//gs;
($VERSION, $RELEASE) = get_release($CHECKOUT);

$TARBALL_PATH = "${PROJECT_NAME}-${VERSION}.tar.gz";
print "Packing source tarball ${TARBALL_PATH} ... ";
system "tar --exclude-vcs --exclude=Drivers --exclude=avrdude --exclude=uploader --xform=s%^${WORK_PATH}%${PROJECT_NAME}-${VERSION}/% -z -c -f ${RPMBUILD_ROOT}/SOURCES/${TARBALL_PATH} ${WORK_PATH}";
print "done\n";

$OLD_SPEC = "${WORK_PATH}/redhat/apmplanner2.spec";
$NEW_SPEC = "${RPMBUILD_ROOT}/SPECS/${PROJECT_NAME}.spec";

print "Writing .spec ... ";
open SPEC_IN, $OLD_SPEC or die $!;
open SPEC_OUT, "> $NEW_SPEC" or die $!;
while (<SPEC_IN>) {
    ~s/^(Version:\s+).*$/$1$VERSION/;
    ~s/^(Release:\s+).*$/$1$RELEASE\%{?dist}/;
    print SPEC_OUT;
}
close SPEC_IN;
close SPEC_OUT;
#rename($NEW_SPEC, $OLD_SPEC);
print "done\n";

sub get_release {
    my $vs = shift;
    $vs =~ /^(\d+(?:\.\d+)+)-(.+)$/;
    my ($v, $r) = ($1, $2);
    $r =~ s/-/./g;
    return ($v, $r);
}
