#!/bin/bash

# script to convert the openclipart cardsets into the FreeDoko one
# uses imagemagick
#
# 12. 11. 2006
# Diether Knof

# height of the destination cards
HEIGHT=138

# the number of rows/columns in the source
ROWS=5
COLS=13

# 1. argument: source (without extension)
# 2. argument: row of the card (start with 0)
# 3. argument: column of the card (start with 0)
# 4. argument: destination file
function convert_card {
  SOURCE=$1.png
  ROW=$2
  COL=$3
  DESTINATION=$4

  total_width=$(identify -format "%w" ${SOURCE})
  total_height=$(identify -format "%h" ${SOURCE})
  WIDTH=$((${total_width} / ${COLS}))

  convert -crop ${WIDTH}x${HEIGHT}+$((${COL}*${WIDTH}))+$((${ROW}*${HEIGHT})) \
	  ${SOURCE} \
	  ${DESTINATION}


  #composite ../mask.${HEIGHT}.png $1 $2
  #mogrify -transparent '#ffff00' $2
} # function convert_card

# first argument: cardset
function convert_cardset
{
  CARDSET=$1
  echo "converting cardset " ${CARDSET}

  # create .png file manually
  #convert -scale $((${ROWS} * ${HEIGHT}))x$((${ROWS} * ${HEIGHT})) ${CARDSET}.svg ${CARDSET}.png
  #echo convert -scale $(( ($(identify -format "%w" ${CARDSET}.png) / ${COLS}) * ${COLS}))x$((${ROWS} * ${HEIGHT})) ${CARDSET}.svg ${CARDSET}.png
  #convert -scale $(( ($(identify -format "%w" ${CARDSET}.png) / ${COLS}) * ${COLS}))x$((${ROWS} * ${HEIGHT})) ${CARDSET}.svg ${CARDSET}.png

  mkdir ${CARDSET}

  echo "  club"
  mkdir -p ${CARDSET}/cards/club
  convert_card ${CARDSET} 0  0 ${CARDSET}/cards/club/ace.png
  convert_card ${CARDSET} 0  8 ${CARDSET}/cards/club/nine.png
  convert_card ${CARDSET} 0  9 ${CARDSET}/cards/club/ten.png
  convert_card ${CARDSET} 0 10 ${CARDSET}/cards/club/jack.png
  convert_card ${CARDSET} 0 11 ${CARDSET}/cards/club/queen.png
  convert_card ${CARDSET} 0 12 ${CARDSET}/cards/club/king.png
  echo "  spade"
  mkdir -p ${CARDSET}/cards/spade
  convert_card ${CARDSET} 3  0 ${CARDSET}/cards/spade/ace.png
  convert_card ${CARDSET} 3  8 ${CARDSET}/cards/spade/nine.png
  convert_card ${CARDSET} 3  9 ${CARDSET}/cards/spade/ten.png
  convert_card ${CARDSET} 3 10 ${CARDSET}/cards/spade/jack.png
  convert_card ${CARDSET} 3 11 ${CARDSET}/cards/spade/queen.png
  convert_card ${CARDSET} 3 12 ${CARDSET}/cards/spade/king.png
  echo "  heart"
  mkdir -p ${CARDSET}/cards/heart
  convert_card ${CARDSET} 2  0 ${CARDSET}/cards/heart/ace.png
  convert_card ${CARDSET} 2  8 ${CARDSET}/cards/heart/nine.png
  convert_card ${CARDSET} 2  9 ${CARDSET}/cards/heart/ten.png
  convert_card ${CARDSET} 2 10 ${CARDSET}/cards/heart/jack.png
  convert_card ${CARDSET} 2 11 ${CARDSET}/cards/heart/queen.png
  convert_card ${CARDSET} 2 12 ${CARDSET}/cards/heart/king.png
  echo "  diamond"
  mkdir -p ${CARDSET}/cards/diamond
  convert_card ${CARDSET} 1  0 ${CARDSET}/cards/diamond/ace.png
  convert_card ${CARDSET} 1  8 ${CARDSET}/cards/diamond/nine.png
  convert_card ${CARDSET} 1  9 ${CARDSET}/cards/diamond/ten.png
  convert_card ${CARDSET} 1 10 ${CARDSET}/cards/diamond/jack.png
  convert_card ${CARDSET} 1 11 ${CARDSET}/cards/diamond/queen.png
  convert_card ${CARDSET} 1 12 ${CARDSET}/cards/diamond/king.png

  mkdir ${CARDSET}/backs
  convert_card ${CARDSET} 4 2 ${CARDSET}/backs/back.png

  mkdir ${CARDSET}/icons
  cd ${CARDSET}/icons
  ~/Programme/FreeDoko/data/bin/create_icons
  cd -
} # function convert_cardset

convert_cardset bellot
#convert_cardset dondorf
#convert_cardset paris
HEIGHT=123
#convert_cardset bonded

#pngcrushmin */*/*.png

