#!/bin/zsh

# Skript, um Symbole zu erzeugen

if [ $# = 0 ]; then
  echo "Argument(e) erwartet: Kartenhöhe"
  echo "Beispiel: $0 140 185 240"
  exit
fi

while [ $# -ge 1 ]; do
  mkdir -p "$1"
  pushd "$1"

  cp ../License.txt .

  HEIGHT=$(( $1 * 4 / 10 ))

  # Hochzeit
  inkscape --export-png=marriage.png --export-height=$((2*${HEIGHT})) --export-background-opacity=0 --without-gui ../rings.svg
  convert marriage.png -trim marriage.png

  inkscape --export-png=marriage.solo.png --export-height=$((2*${HEIGHT})) --export-background-opacity=0 --without-gui ../rings1.svg
  convert marriage.solo.png -trim marriage.solo.png

  inkscape --export-png=club.png --export-height=$((${HEIGHT}*9/10)) --export-background-opacity=0 --without-gui ../SuitClubs.svg
  inkscape --export-png=spade.png --export-height=$((${HEIGHT}*9/10)) --export-background-opacity=0 --without-gui ../SuitSpades.svg
  inkscape --export-png=heart.png --export-height=$((${HEIGHT}*9/10)) --export-background-opacity=0 --without-gui ../SuitHearts.svg
  inkscape --export-png=trump.png --export-height=$((${HEIGHT}*9/10)) --export-background-opacity=0 --without-gui ../SuitDiamonds.svg

  for f in club spade heart trump; do 
    convert -alpha Set -background none \
      -gravity Center \
      marriage.png $f.png \
      -compose dst-over -composite \
      marriage.$f.png
  done

  inkscape --export-png=club.png --export-height=$((${HEIGHT}/2)) --export-background-opacity=0 --without-gui ../SuitClubs.svg
  inkscape --export-png=spade.png --export-height=$((${HEIGHT}/2)) --export-background-opacity=0 --without-gui ../SuitSpades.svg
  inkscape --export-png=heart.png --export-height=$((${HEIGHT}/2)) --export-background-opacity=0 --without-gui ../SuitHearts.svg
  inkscape --export-png=trump.png --export-height=$((${HEIGHT}/2)) --export-background-opacity=0 --without-gui ../SuitDiamonds.svg

  # erster Farb
  convert -alpha Set -background none \
    club.png heart.png spade.png +append \
    marriage.color.png
  convert -alpha Set -background none \
    -gravity Center \
    marriage.png \
    marriage.color.png \
    -compose dst-over -composite \
    marriage.color.png

  # erster Fremde
  convert -alpha Set -background none \
    \( trump.png club.png +append \) \
    \( spade.png heart.png +append \) \
    -append \
    marriage.foreign.png
  convert -alpha Set -background none \
    -gravity Center \
    marriage.png \
    marriage.foreign.png \
    -compose dst-over -composite \
    marriage.foreign.png

  rm club.png spade.png heart.png trump.png

  # Schweine
  inkscape --export-png=swine.png --export-height=${HEIGHT} --export-background-opacity=0 --without-gui ../lalolalo_Smiling_pig_color.svg
  inkscape --export-png=hyperswine.png --export-height=${HEIGHT} --export-background-opacity=0 --without-gui ../liftarn_Running_pig.svg

  convert -alpha Set -background none \
    swine.png -page +$(($HEIGHT / 2))+0 swine.png \
    -layers merge +repage \
    swines.png

  convert -alpha Set -background none \
    hyperswine.png -page -$(($HEIGHT / 2))+0 hyperswine.png \
    -layers merge +repage \
    hyperswines.png

  convert -alpha Set -background none \
    hyperswines.png -page -$(($HEIGHT * 4/5))+0 swines.png \
    -layers merge +repage \
    swines-hyperswines.png


  rm swine.png hyperswine.png
  popd >/dev/null
  shift
done
