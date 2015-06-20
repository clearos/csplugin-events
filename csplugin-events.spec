# ClearSync System Monitor Plugin RPM spec
%{!?_with_pic: %{!?_without_pic: %define _with_pic --with-pic=inih}}

Name: csplugin-events
Version: 1.0
Release: 13%{dist}
Vendor: ClearFoundation
License: GPL
Group: System/Plugins
Packager: ClearFoundation
Source: %{name}-%{version}.tar.gz
BuildRoot: /var/tmp/%{name}-%{version}
Requires: clearsync >= 1.8
BuildRequires: clearsync-devel >= 2.0
BuildRequires: expat-devel
BuildRequires: sqlite-devel
BuildRequires: openssl-devel
BuildRequires: autoconf >= 2.63
BuildRequires: automake
BuildRequires: libtool
Summary: ClearSync System Monitor plugin
Requires(pre): /sbin/ldconfig, /usr/sbin/groupmems

%description
This is an System Monitor ClearSync plugin.
Report bugs to: http://www.clearfoundation.com/docs/developer/bug_tracker/

# Build
%prep
%setup -q
./autogen.sh
%configure %{?_with_pic}

%build
make %{?_smp_mflags}

# Install
%install
make install DESTDIR=$RPM_BUILD_ROOT
rm -f ${RPM_BUILD_ROOT}/%{_libdir}/lib*.a
rm -f ${RPM_BUILD_ROOT}/%{_libdir}/lib*.la
mkdir -vp ${RPM_BUILD_ROOT}/%{_sysconfdir}/clearsync.d
cp -v csplugin-events.conf ${RPM_BUILD_ROOT}/%{_sysconfdir}/clearsync.d
mkdir -vp ${RPM_BUILD_ROOT}/%{_localstatedir}/lib/csplugin-events
touch ${RPM_BUILD_ROOT}/%{_localstatedir}/lib/csplugin-events/events.db
mkdir -vp ${RPM_BUILD_ROOT}/%{_sysconfdir}/rsyslog.d
cp -v deploy/rsyslog.conf ${RPM_BUILD_ROOT}/%{_sysconfdir}/10-clearsync.conf

# Clean-up
%clean
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT

# Pre install
%pre
/usr/sbin/groupmems -g webconfig -a clearsync 2> /dev/null || :

# Post install
%post
/sbin/ldconfig
%if "0%{dist}" == "0.v6"
/sbin/service clearsyncd condrestart >/dev/null 2>&1 || :
%else
/sbin/service clearsync condrestart >/dev/null 2>&1 || :
%endif
/sbin/service rsyslog condrestart >/dev/null 2>&1 || :

# Add command(s) to sudo configuration
#CHECK=`grep "^clearsync[[:space:]]*" /etc/sudoers`
#if [ -z "$CHECK" ]; then
#    echo "Cmnd_Alias CLEARSYNC = " >> /etc/sudoers
#	echo "clearsync ALL=NOPASSWD: CLEARSYNC" >> /etc/sudoers
#fi

#CMD=/sbin/service
#LINE=`grep "^Cmnd_Alias CLEARSYNC" /etc/sudoers 2>/dev/null`
#CHECK=`echo $LINE, | grep $CMD,`
#if [ -z "$CHECK" ]; then
#	ESCAPE=`echo $CMD | sed 's/\//\\\\\//g'`
#	sed -i -e "s/Cmnd_Alias CLEARSYNC.*=/Cmnd_Alias CLEARSYNC = $ESCAPE,/i" /etc/sudoers
#	sed -i -e "s/[[:space:]]*,[[:space:]]*$//i" /etc/sudoers
#	chmod 440 /etc/sudoers
#fi

# Post uninstall
%postun
/sbin/ldconfig
%if "0%{dist}" == "0.v6"
/sbin/service clearsyncd condrestart >/dev/null 2>&1 || :
%else
/sbin/service clearsync condrestart >/dev/null 2>&1 || :
%endif
/sbin/service rsyslog condrestart >/dev/null 2>&1 || :

# Files
%files
%defattr(-,root,root)
%config(noreplace) %{_sysconfdir}/clearsync.d/csplugin-events.conf
%{_libdir}/libcsplugin-events.so*
%attr(770,clearsync,webconfig) %{_localstatedir}/lib/csplugin-events/
%attr(660,clearsync,webconfig) %{_localstatedir}/lib/csplugin-events/events.db
%{_bindir}/eventsctl
%attr(644,root,root) %{_sysconfdir}/rsyslog.d/10-clearsync.conf
