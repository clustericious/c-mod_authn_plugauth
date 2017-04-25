Name: mod_authn_plugauth
Version: 0.01
Release: 3
Summary: PlugAuth apache module

License: BSD
URL: https://github.com/plicease/mod_authn_plugauth
Source0: mod_authn_plugauth-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-BUILD

%description
PlugAuth apache module

%define myfilelist %{_tmppath}/filelist-%(%{__id_u} -n)

%prep
%setup -q -n mod_authn_plugauth-%{version}

%build
make

%install
make install PREFIX=/util DESTDIR=$RPM_BUILD_ROOT
find %{buildroot} -not -type d | sed -e 's#%{buildroot}##' > %{myfilelist}

%clean
rm -rf $RPM_BUILD_ROOT

%files -f %{myfilelist}
%defattr(-,root,root,-)
