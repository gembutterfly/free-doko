#!/bin/zsh

if [ ! $# -eq 2 ]; then
  echo "Zwei Argumente (Breite und Höhe) übergeben."
  exit
fi
BREITE=$1
HOEHE=$2
GROESSE=${BREITE}x${HOEHE}

pushd $(dirname "$0") >/dev/null


# Tux-Bild erzuegen
if false; then
  # erzeugt einen weißen Streifen :-(
convert +antialias -background transparent \
  -scale $(($BREITE*7/8))x$(($HOEHE*7/8)) Tux.svg \
  tux-tmp.png
else
  inkscape Tux.svg -z --export-width=$(($BREITE*7/8)) --export-png=tux-tmp.png
fi
TUX_BREITE=$(identify -format "%w" tux-tmp.png)
TUX_HOEHE=$(identify -format "%h" tux-tmp.png)

# Farbverlauf erstellen
# Tux hinzufügen
# Spielkartensymbole hinzufügen
convert +antialias -background transparent -alpha Set \
  -gravity Center \
  -size $GROESSE gradient:'#6cf-#008' \
  tux-tmp.png \
  -geometry +0-$(($TUX_HOEHE/8)) -composite \
  \( -scale $(($BREITE*3/17))x SuitClubs.svg \)    -geometry -$(($BREITE*3/9))+$(($HOEHE*1/3)) -composite \
  \( -scale $(($BREITE*3/17))x SuitSpades.svg \)   -geometry -$(($BREITE*1/9))+$(($HOEHE*1/3)) -composite \
  \( -scale $(($BREITE*3/17))x SuitHearts.svg \)   -geometry +$(($BREITE*1/9))+$(($HOEHE*1/3)) -composite \
  \( -scale $(($BREITE*3/17))x SuitDiamonds.svg \) -geometry +$(($BREITE*3/9))+$(($HOEHE*1/3)) -composite \
  -set author "Dr. Diether Knof" \
  -set license "siehe Wikipedia" \
  -set title "Pinguin Kartenrückseite" \
  tux.png

popd >/dev/null
exit

pushd $(dirname "$0") >/dev/null
# Rahmen hinzufügen

# Bildmaske erzeugen
convert -size $GROESSE \
  xc:transparent -fill transparent -stroke black \
  -draw "roundrectangle 0,0 $(($BREITE-1)),$(($HOEHE-1)) $(($BREITE/10)),$(($HOEHE/10))" \
  border.png

convert -size $GROESSE \
  xc:transparent -fill black -stroke black \
  -draw "roundrectangle 0,0 $(($BREITE-1)),$(($HOEHE-1)) $(($BREITE/10)),$(($HOEHE/10))" \
  mask.png

convert +antialias -background transparent -alpha Set \
  tux.png \
  -compose Dst_In mask.png -composite \
  -compose Over border.png -composite \
  tux.png

popd >/dev/null
