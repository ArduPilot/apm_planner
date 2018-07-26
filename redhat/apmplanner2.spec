Summary:            APM Planner - Micro Air Vehicle Groundstation
Name:               apmplanner2
Version:            2.0.25
Release:            rc1.11.g7a23a1c%{?dist}
License:            GPLv3
Group:              Applications/Science/Engineering
Source:             %{name}-%{version}.tar.gz
Patch0:             %{name}-0.patch
Patch1:             %{name}-1.patch
BuildRequires:      gcc-c++
BuildRequires:      make
BuildRequires:      mesa-libGL-devel
BuildRequires:      qt5-qtbase-devel >= 5.2
BuildRequires:      qt5-qtdeclarative-devel >= 5.2
BuildRequires:      qt5-qtscript-devel >= 5.2
BuildRequires:      qt5-qtserialport-devel >= 5.2
BuildRequires:      qt5-qtsvg-devel >= 5.2
BuildRequires:      qt5-qtwebkit-devel >= 5.2
BuildRequires:      SDL2-devel >= 2.0
BuildRequires:      alsa-lib-devel
BuildRequires:      flite-devel
BuildRequires:      libsndfile-devel
BuildRequires:      openssl-devel
BuildRequires:      zlib-devel
URL:                https://github.com/diydrones/apm_planner
ExcludeArch:        s390 s390x

%description

%define debug_package %{nil}

%prep
%autosetup

%build
qmake-qt5 CONFIG+=%{_arch} apm_planner.pro PREFIX=$RPM_BUILD_ROOT%{_prefix}

NCORES=`cat /proc/cpuinfo | grep ^processor | sort -r | head -n 1 | sed -e 's/^.\+\([0-9]\)\+.*/\1/'`
NPROCS=$(( ( $NCORES + 1 ) * 2 ));

make -j$NPROCS

%install
%make_install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%{_bindir}/*
%{_datadir}/*

%changelog

