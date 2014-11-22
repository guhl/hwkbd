#
# hwkbd-daemon spec
# (C) Guhl 2014 
# based on tohkbd2-daemon by kimmoli
#

Name: hwkbd

%{!?qtc_qmake:%define qtc_qmake %qmake}
%{!?qtc_qmake5:%define qtc_qmake5 %qmake5}
%{!?qtc_make:%define qtc_make make}
%{?qtc_builddir:%define _builddir %qtc_builddir}
Summary: Hardware keyboard daemon
Version: 0.0.devel
Release: 1
Group:   Qt/Qt
License: LICENSE
URL:     https://github.com/guhl/hwkbd-daemon
Source0: %{name}-%{version}.tar.bz2

BuildRequires: pkgconfig(Qt5Core)
BuildRequires: pkgconfig(Qt5DBus)
BuildRequires: pkgconfig(mlite5)

%description
Daemon for the hardware keyboard

%prep
%setup -q -n %{name}-%{version}

%build

%qtc_qmake5 SPECVERSION=%{version}

%qtc_make %{?_smp_mflags}

%install
rm -rf %{buildroot}

%qmake5_install

%files
%defattr(644,root,root,755)
%attr(6711,root,root) %{_bindir}/%{name}
%config /etc/systemd/user/%{name}.service
%config /etc/dbus-1/system.d/%{name}.conf
%config /usr/share/maliit/plugins/com/jolla/layouts/%{name}.conf
%config /usr/share/maliit/plugins/com/jolla/layouts/%{name}.qml

%post
#reload udev rules
udevadm control --reload
systemctl-user start %{name}.service

%pre
# In case of update, stop and disable first
if [ "$1" = "2" ]; then
  systemctl-user stop %{name}.service
  systemctl-user disable %{name}.service
  udevadm control --reload
fi

%preun
# in case of complete removal, stop and disable
if [ "$1" = "0" ]; then
  systemctl-user stop %{name}.service
  systemctl-user disable %{name}.service
  udevadm control --reload
fi

