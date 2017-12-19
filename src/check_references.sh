#!/bin/sh

# Überprüfe die Referenzen


if [ $# -eq 0 ]; then
  DIR="../../references"
else
  DIR="$1"
fi


cd "$DIR"
# nach Bugreport.FreeDoko suchen
x=$(find -name "*.BugReport.FreeDoko" | sed "s,^\./,,")
echo $x
if [ -n "$x" ]; then
  echo "Im Referenzverzeichnis existieren Dateien *.BugReport.FreeDoko:"
  echo "$x"
  exit
fi
# nach CHECK suchen
x=$(grep -l CHECK . -r)
if [ -n "$x" ]; then
  echo "Im Referenzverzeichnis existieren Dateien mit „CHECK“:"
  echo "$x"
  exit
fi
# nach FreeDoko BugReport suchen
x=$(find -name "*.Reference.FreeDoko" \
  | sed "s,^\./,," \
  | while read f; do \
< $f head -n 1 | grep -vq "FreeDoko Reference" && echo "$f"; \
done)
if [ -n "$x" ]; then
  echo "Im Referenzverzeichnis existieren Dateien ohne „FreeDoko Reference“:"
  echo "$x"
  exit
fi
cd -


test -f FreeDoko && rm FreeDoko

mkdir -p "/tmp/${LOGNAME}/FreeDoko.check_reference/src"

#CXXFLAGS="-Wall -Werror -Wno-parentheses -pipe -O0 -ggdb"
CXXFLAGS="-Wall -Werror -Wno-parentheses -pipe -O2 -std=c++14" \
 make \
 USE_UI_GTKMM=false \
 USE_UI_TEXT=false \
 USE_SOUND=false \
 USE_BOOST=true \
 FREEDOKO_WORKING_DIRECTORY=/tmp/${LOGNAME}/FreeDoko.check_reference/src

test -f ./FreeDoko || exit

cd ../data
../src/FreeDoko -r "$DIR" --ui=none

cd "$DIR"

grep -h -v OK References.report.csv \
  | grep "^[^;]*[0-9]\+\(_[0-9]\+\)\?.Reference.FreeDoko" \
  | sort -n

tail -n 5 References.report.csv \
  | sed "s/\;/\t/g";

echo

x=$(find . -name "*.Reference.FreeDoko" \
  | sed "s,^\./,," \
  | while read f; do
grep -q "^$f;" References.report.csv || echo "$f"
done \
  | wc -l)
if [ ! $x -eq 0 ]; then
echo -n "Dateien gesamt:        "
find . -name "*.Reference.FreeDoko" | wc -l
  echo "Dateien nicht geprüft: $x"
  exit
fi
echo -n "Dateien gesamt:     "
find . -name "*.Reference.FreeDoko" | wc -l

echo
echo "Einträge überprüft: "`grep "^[ \t]*check" *.Reference.FreeDoko **/*.Reference.FreeDoko | wc -l`

# Die kaputten Referenzen in ein separates Verzeichnis kopiert
rm -rf /tmp/FreeDoko.References.failed
mkdir /tmp/FreeDoko.References.failed
grep -v "^\([^;]*;\)\{3\}OK;" References.report.csv \
  | sed "1d" \
  | sed "s/;.*$//" \
  | sed "/^$/d" \
  | sed "/^files$/d" \
  | sed "/^checks$/d" \
  | sed "/^success$/d" \
  | sed "/^failed$/d" \
  >/tmp/FreeDoko.References.failed/References.failed

echo -n "Kaputt:             "
wc -l </tmp/FreeDoko.References.failed/References.failed
x=$(grep -v "_broken" /tmp/FreeDoko.References.failed/References.failed)
if [ -n "$x" ]; then
  /bin/echo -n -e "\e[01;31m"
  echo "Neu kaputt:         $(echo $x | wc -l)"
  echo "$x"
  /bin/echo -n -e "\e[;0m"
else
  /bin/echo -n -e "\e[01;32m"
  echo "Neu kaputt:         0"
  /bin/echo -n -e "\e[;0m"
fi

for f in References.report.csv `cat /tmp/FreeDoko.References.failed/References.failed`; do
  mkdir -p /tmp/FreeDoko.References.failed/"`dirname $f`"
  cp "$f" "/tmp/FreeDoko.References.failed/$f"
done

# Die wieder funktionierenden kaputten Referenzen anzeigen
x=$(find . -name "*.Reference.FreeDoko" \
  | sed "s,^\./,," \
  | grep "_broken" \
  | while read f; do
grep -q "$f" /tmp/FreeDoko.References.failed/References.failed \
  || echo "$f" | sed "s/^/  /"
done)
if [ -n "$x" ]; then
  /bin/echo -n -e "\e[01;31m"
  echo "Wieder heil:        $(echo $x | wc -l)"
  echo "$x x" | sed "s/^/  /"
  /bin/echo -n -e "\e[;0m"
else
  /bin/echo -n -e "\e[01;32m"
  echo "Wieder heil:        0"
  /bin/echo -n -e "\e[;0m"
fi
