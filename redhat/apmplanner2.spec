Summary:            APM Planner - Micro Air Vehicle Groundstation
Name:               apmplanner2
Version:            2.0.16
Release:            rc1.32.gc208324%{?dist}
License:            GPLv3
Group:              Applications/Science/Engineering
Source:             %{name}-%{version}.tar.gz
#Patch0:             %{name}-0.patch
BuildRequires:      make
BuildRequires:      cmake
BuildRequires:      ninja-build
BuildRequires:      mesa-libGL-devel
BuildRequires:      qt6-qtbase-devel >= 6.2
BuildRequires:      qt6-qtdeclarative-devel >= 6.2
BuildRequires:      qt6-qtserialport-devel >= 6.2
BuildRequires:      qt6-qtsvg-devel >= 6.2
BuildRequires:      qt6-qtmultimedia-devel >= 6.2
BuildRequires:      qt6-qt5compat-devel >= 6.2
BuildRequires:      qt6-qtdatavis3d-devel >= 6.2
BuildRequires:      SDL2-devel >= 2.0
BuildRequires:      alsa-lib-devel
#BuildRequires:      flite-devel
BuildRequires:      libsndfile-devel
BuildRequires:      openssl-devel
BuildRequires:      zlib-devel
URL:                https://github.com/ardupilot/apm_planner
ExcludeArch:        s390 s390x

%description

%global debug_package %{nil}

%prep
%autosetup

%build
# CMake is the canonical build; it auto-detects Qt6 (preferred) or Qt5.
%cmake -DCMAKE_BUILD_TYPE=Release
%cmake_build

%install
%cmake_install

%clean
rm -rf $RPM_BUILD_ROOT

%files

%{_bindir}/*
%{_datadir}/*

%changelog

