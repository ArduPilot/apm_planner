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
    $WORK_PATH = "apm_planner";
}
$WORK_PATH = `realpath ${WORK_PATH}`;
chomp($WORK_PATH);

if (-d $WORK_PATH && -d "${WORK_PATH}/.git") {
    system "git -C ${WORK_PATH} pull"
} else {
    system "git clone ${REPO_URL} ${WORK_PATH}";
}

$CHECKOUT = `git -C ${WORK_PATH} describe`;
$CHECKOUT =~ s/\n.*//gs;
($VERSION, $RELEASE) = get_release($CHECKOUT);

$TARBALL_PATH = "${RPMBUILD_ROOT}/SOURCES/${PROJECT_NAME}-${VERSION}.tar.gz";
unlink $TARBALL_PATH;
print "Packing source tarball ${TARBALL_PATH} ... ";
($WORK_PATH_REGEX = $WORK_PATH) =~ s/^\///;
system "tar --exclude-vcs --exclude=Drivers --exclude=avrdude --exclude=uploader --xform=s%^${WORK_PATH_REGEX}%${PROJECT_NAME}-${VERSION}/% -z -c -f ${TARBALL_PATH} ${WORK_PATH}";
print "done\n";

$OLD_SPEC = "${WORK_PATH}/redhat/apmplanner2.spec";
$NEW_SPEC = "${RPMBUILD_ROOT}/SPECS/${PROJECT_NAME}.spec";

print "Writing ${PROJECT_NAME}.spec ... ";
open SPEC_IN, $OLD_SPEC or die $!;
open SPEC_OUT, "> $NEW_SPEC" or die $!;
while (<SPEC_IN>) {
    ~s/^(Version:\s+).*$/$1$VERSION/;
    ~s/^(Release:\s+).*$/$1$RELEASE\%{?dist}/;
    print SPEC_OUT;
}
close SPEC_IN;
close SPEC_OUT;
print "done\n";

print "Copying diydrones.repo ... ";
$REPO_FILE = "${WORK_PATH}/redhat/diydrones.repo";
system "cp ${REPO_FILE} ${RPMBUILD_ROOT}/SOURCES";
print "done\n";

print "Copying diydrones-repo.spec ... ";
$OLD_SPEC = "${WORK_PATH}/redhat/diydrones-repo.spec";
system "cp ${OLD_SPEC} ${RPMBUILD_ROOT}/SPECS";
print "done\n";

sub get_release {
    my $vs = shift;
    $vs =~ /^(\d+(?:\.\d+)+)-(.+)$/;
    my ($v, $r) = ($1, $2);
    $r =~ s/-/./g;
    return ($v, $r);
}
