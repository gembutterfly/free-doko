# norootforbuild
# neededforbuild update-desktop-files gtkmm2-devel gtkmm2 asciidoc dos2unix w3m pango-devel pango glib2-devel glib2 libstdc++-devel libstdc++ gnet-devel gnet zlib-devel zlib

%define _prefix      /usr
%define _cvs_build   1
%define _cvs_version cvs20060302

Name:          FreeDoko
%if "%{_cvs_build}" == "1"
Version:       0.7.3
Release:       0.oc2pus.%{_cvs_version}
%else
Version:       0.7.2b
Release:       0.oc2pus.1
%endif
Summary:       FreeDoko is a Doppelkopf-game
Summary(de):   FreeDoko ist ein Doppelkopfspiel
Group:         Amusements/Games/Board/Card
License:       GPL
Packager:      oc2pus <oc2pus@arcor.de>
URL:           http://free-doko.sourceforge.net/de/index.html
%if "%{_cvs_build}" == "1"
Source0:       %{name}-%{version}.tar.bz2
Source1:       %{name}-src-Makefile.local
Source2:       freedoko.asciidoc
Patch1:        %{name}-data-cardsets-Makefile.diff
NoSource:      0
%else
Source0:       %{name}_%{version}.src.tar.bz2
Source1:       %{name}-src-Makefile.local
Patch0:        %{name}-Makefile.diff
%endif
BuildRoot:     %{_tmppath}/%{name}-%{version}-build
BuildRequires: gtkmm2-devel
BuildRequires: asciidoc
BuildRequires: dos2unix
BuildRequires: te_latex
BuildRequires: w3m
BuildRequires: libstdc++-devel
BuildRequires: pango-devel
BuildRequires: glib2-devel
BuildRequires: gnet-devel
BuildRequires: zlib-devel
BuildRequires: cairo-devel
Requires:      gtkmm2
#Requires:      libstdc++
Requires:      pango
Requires:      glib2
Requires:      gnet
Requires:      zlib
Requires:      cairo

%description
FreeDoko is a Doppelkopf-game, written by Borg Enders and Diether Knof.
It is developed under and for the platforms Linux and Windows
The game is still under developement, but since some time playable.

%if "%{_cvs_build}" == "1"
Snapshot: %_cvs_version

%endif
Authors:
FreeDoko developers <free-doko-developer@lists.sourceforge.net>
Borg Enders <ben@BorgSoft.de>
Diether Knof <dknof@gmx.de>

%description -l de
FreeDoko ist ein Doppelkopfspiel, geschrieben von Borg Enders und Diether Knof.
Es wird unter den und für die Plattformen Linux und Windows geschrieben.
Es ist noch in der Entwicklung, aber schon längere Zeit spielbar.

%if "%{_cvs_build}" == "1"
Snapshot: %_cvs_version

%endif
Autoren:
FreeDoko developers <free-doko-developer@lists.sourceforge.net>
Borg Enders <ben@BorgSoft.de>
Diether Knof <dknof@gmx.de>

%package     manual
Summary:     User documetation for %{name}
Group:       Documentation/Other

%description manual
Usermanual for %{name}.

%if "%{_cvs_build}" == "1"
Snapshot: %_cvs_version

%endif
Autoren:
FreeDoko developers <free-doko-developer@lists.sourceforge.net>
Borg Enders <ben@BorgSoft.de>
Diether Knof <dknof@gmx.de>

%description -l de manual
Benutzerhandbuch für %{name}.

%if "%{_cvs_build}" == "1"
Snapshot: %_cvs_version

%endif
Autoren:
FreeDoko developers <free-doko-developer@lists.sourceforge.net>
Borg Enders <ben@BorgSoft.de>
Diether Knof <dknof@gmx.de>

%prep
%if "%{_cvs_build}" == "1"
%setup -q -n %{name}
%patch1
%else
%setup -q -n %{name}_%{version}
%patch0
%endif

%if "%{_cvs_build}" == "1"
# TODO: should be in normal svn-tarball
install -dm 755 debian
install -m 644 %{SOURCE2} debian
%endif

# Makefile.local
install -m 644 %{SOURCE1} src/Makefile.local

# dos2unix
for i in doc/manual/de/credits.html \
	doc/manual/de/overview.html \
	doc/manual/de/index.html \
	doc/manual/de/shortrules.html \
	doc/manual/de/network_protocol.html \
	doc/manual/de/faq.html \
	doc/manual/de/configuration.html \
	doc/manual/de/installation.html \
	doc/manual/de/files.html \
	doc/manual/de/introduction.html \
	doc/manual/de/rules.html \
	doc/manual/en/index.html \
	doc/manual/en/credits.html \
	doc/manual/en/faq.html \
	doc/manual/en/rules.html \
	doc/manual/en/introduction.html \
	doc/manual/en/configuration.html \
	doc/manual/en/operation/index.html \
	doc/manual/en/overview.html \
	doc/manual/en/operation/overview.html \
	doc/manual/en/operation/ai_config.html \
	doc/manual/en/shortrules.html \
	doc/manual/index.html \
	doc/de/Windows \
	doc/de/LIESMICH.txt \
	doc/en/Windows \
	doc/en/README.txt \
	doc/en/README.html \
	COPYING \
	AUTHORS; do
	dos2unix  $i
	chmod 644 $i
done

%build

%install
[ -d %{buildroot} -a "%{buildroot}" != "" ] && rm -rf %{buildroot}

make install \
	DESTDIR=%{buildroot}

# strip large binary
%__strip %{buildroot}%{_bindir}/%{name}

# move documentations
install -dm 755 %{buildroot}%{_docdir}
rm -f %{buildroot}%{_datadir}/doc/%{name}
mv %{buildroot}%{_datadir}/doc/freedoko %{buildroot}%{_datadir}/doc/%{name}
mv %{buildroot}%{_datadir}/doc/%{name}  %{buildroot}%{_docdir}

rm -f %{buildroot}%{_datadir}/games/%{name}/doc
%__ln_s %{_docdir}/%{name} %{buildroot}%{_datadir}/games/%{name}/doc

# menu-entry
cat > %{name}.desktop << EOF
[Desktop Entry]
Type=Application
Name=%{name}
Name[de]=Doppelkopf
Name[en]=%{name}
Comment[de]=FreeDoko ist ein Doppelkopfspiel
Comment[en]=FreeDoko is a Doppelkopf-game
Comment=FreeDoko is a Doppelkopf-game
Exec=%{name}.sh
Icon=%{name}
EOF
%suse_update_desktop_file -i %{name} Game CardGame

# startscript
cat > %{name}.sh << EOF
#!/bin/bash
cd %{_datadir}/games/%{name} && %{_bindir}/%{name}
EOF

install -dm 755 %{buildroot}%{_bindir}
install -m 755 %{name}.sh %{buildroot}%{_bindir}

%clean
[ -d %{buildroot} -a "%{buildroot}" != "" ] && rm -rf %{buildroot}

%files
%defattr(-, root, root)
%dir %{_docdir}/%{name}
%exclude %{_docdir}/%{name}/manual
%{_docdir}/%{name}/*
%{_bindir}/*
%dir %{_datadir}/games/%{name}
%{_datadir}/games/%{name}/*
%{_mandir}/man6/*
%{_datadir}/pixmaps/%{name}.png
%{_datadir}/applications/%{name}.desktop

%files manual
%defattr(-, root, root)
%dir %{_docdir}/%{name}/manual
%{_docdir}/%{name}/manual/*

%changelog
%if "%{_cvs_build}" == "1"
* Thu Mar 02 2006 oc2pus <oc2pus@arcor.de> 0.7.3-0.oc2pus.cvs20060302
- update to 0.7.3 (cvs 02.03.2006)
%else
* Sat Feb 26 2006 oc2pus <oc2pus@arcor.de> 0.7.2b-0.oc2pus.1
- update to 0.7.2b
- added Makefile.local
- repacked as tar.bz2
* Thu Aug 18 2005 oc2pus <oc2pus@arcor.de> 0.6.9-0.oc2pus.1
- Initial rpm build 0.6.9
%endif
