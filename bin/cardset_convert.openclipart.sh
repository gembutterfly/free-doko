#!/bin/bash

# script to convert the openclipart cardsets into the FreeDoko one
# uses imagemagick
#
# 6. 11. 2006
# Diether Knof

# height of the destination cards
HEIGHT=138
WIDTH=102

rm -rf png.${HEIGHT}

mkdir png.${HEIGHT}



# first argument:  source
# second argument: destination
function convert_card {
  convert -scale x${HEIGHT} \
	  $1 \
	  $2.png

  composite ../mask.${WIDTH}x${HEIGHT}.png $2.png $2
  rm $2.png
  mogrify -transparent '#ffff00' $2
} # function convert_card

function convert_openclipart {
echo "converting openclipart cardset"

echo "  club"
mkdir -p png.${HEIGHT}/cards/club
convert_card *c_q.svg  png.${HEIGHT}/cards/club/queen.png
convert_card *c_j.svg  png.${HEIGHT}/cards/club/jack.png
convert_card *c_k.svg  png.${HEIGHT}/cards/club/king.png
convert_card *c_a.svg  png.${HEIGHT}/cards/club/ace.png
convert_card *c_9.svg  png.${HEIGHT}/cards/club/nine.png
convert_card *c_10.svg png.${HEIGHT}/cards/club/ten.png
echo "  spade"
mkdir -p png.${HEIGHT}/cards/spade
convert_card *s_q.svg  png.${HEIGHT}/cards/spade/queen.png
convert_card *s_j.svg  png.${HEIGHT}/cards/spade/jack.png
convert_card *s_k.svg  png.${HEIGHT}/cards/spade/king.png
convert_card *s_a.svg  png.${HEIGHT}/cards/spade/ace.png
convert_card *s_9.svg  png.${HEIGHT}/cards/spade/nine.png
convert_card *s_10.svg png.${HEIGHT}/cards/spade/ten.png
echo "  heart"
mkdir -p png.${HEIGHT}/cards/heart
convert_card *h_q.svg  png.${HEIGHT}/cards/heart/queen.png
convert_card *h_j.svg  png.${HEIGHT}/cards/heart/jack.png
convert_card *h_k.svg  png.${HEIGHT}/cards/heart/king.png
convert_card *h_a.svg  png.${HEIGHT}/cards/heart/ace.png
convert_card *h_9.svg  png.${HEIGHT}/cards/heart/nine.png
convert_card *h_10.svg png.${HEIGHT}/cards/heart/ten.png
echo "  diamond"
mkdir -p png.${HEIGHT}/cards/diamond
convert_card *d_q.svg  png.${HEIGHT}/cards/diamond/queen.png
convert_card *d_j.svg  png.${HEIGHT}/cards/diamond/jack.png
convert_card *d_k.svg  png.${HEIGHT}/cards/diamond/king.png
convert_card *d_a.svg  png.${HEIGHT}/cards/diamond/ace.png
convert_card *d_9.svg  png.${HEIGHT}/cards/diamond/nine.png
convert_card *d_10.svg png.${HEIGHT}/cards/diamond/ten.png

mkdir png.${HEIGHT}/backs
for f in ../cardbacks/*.svg; do 
  convert_card $f  png.${HEIGHT}/backs/`basename $f .svg`.png
done

mkdir png.${HEIGHT}/icons
cd png.${HEIGHT}/icons
~/Programme/FreeDoko/data/bin/create_icons
cd ../..
} # function convert_openclipart

convert_openclipart

pngcrushmin */*/*.png
