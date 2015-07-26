Summary:            DIY Drones Repository Configuration
Name:               diydrones-repo
Version:            1
Release:            1
License:            BSD
Group:              System Environment/Base
URL:                https://github.com/diydrones/apm_planner
Source1:            diydrones.repo

%description
Enable DIY Drones repository on your box.

%prep
echo "Nothing to prep"

%build
echo "Nothing to build"

%install

# Create dirs
#install -d -m755 \
#  $RPM_BUILD_ROOT%{_sysconfdir}/pki/rpm-gpg  \
#  $RPM_BUILD_ROOT%{_sysconfdir}/yum.repos.d
install -d -m755 \
    $RPM_BUILD_ROOT%{_sysconfdir}/yum.repos.d

# GPG Key
#%{__install} -Dp -m644 \
#    %{SOURCE21} \
#    %{SOURCE22} \
#    %{SOURCE23} \
#    $RPM_BUILD_ROOT%{_sysconfdir}/pki/rpm-gpg

# Avoid using basearch in name for the key. Introduced in F18
#ln -s $(basename %{SOURCE21}) $RPM_BUILD_ROOT%{_sysconfdir}/pki/rpm-gpg/RPM-GPG-KEY-rpmfusion-%{repo}-fedora-21
#ln -s $(basename %{SOURCE22}) $RPM_BUILD_ROOT%{_sysconfdir}/pki/rpm-gpg/RPM-GPG-KEY-rpmfusion-%{repo}-fedora-22
#ln -s $(basename %{SOURCE23}) $RPM_BUILD_ROOT%{_sysconfdir}/pki/rpm-gpg/RPM-GPG-KEY-rpmfusion-%{repo}-fedora-23

# Links for the keys
#ln -s $(basename %{SOURCE22}) $RPM_BUILD_ROOT%{_sysconfdir}/pki/rpm-gpg/RPM-GPG-KEY-rpmfusion-%{repo}-fedora-latest
#ln -s $(basename %{SOURCE23}) $RPM_BUILD_ROOT%{_sysconfdir}/pki/rpm-gpg/RPM-GPG-KEY-rpmfusion-%{repo}-fedora-rawhide


# Yum .repo files
%{__install} -p -m644 %{SOURCE1} \
    $RPM_BUILD_ROOT%{_sysconfdir}/yum.repos.d

%files
#%{_sysconfdir}/pki/rpm-gpg/*
%config(noreplace) %{_sysconfdir}/yum.repos.d/*

%changelog
