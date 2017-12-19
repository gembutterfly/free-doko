#!/bin/bash
#
# converts kdecarddecks to FreeDoko-cardsets
# uses imagemagick
#
# ./kdecarddecks-to-FreeDoko-cardsets  /usr/share/apps/carddecks/cards-*
#
# 18. June 2002
# Diether Knof

# copying the copyright
mkdir -p kdecarddecks
cp /usr/share/doc/kdegames-card-data/copyright kdecarddecks/

CREATE_ICONS="$(pwd)/create_icons"

while [ $# -gt 0 ]; do
  if [ ! -d $1 ]; then
    echo \"$1\" is not a directory
    shift
    continue
  fi

  style=$(basename $1 | sed "s/^cards-//")

  if [ ! -f $1/1.png ]; then
    echo \"$1\" is not a kdecarddecks directory
    shift
    continue
  fi

  width=$(identify -format "%w" "$1/1.png")
  height=$(identify -format "%h" "$1/1.png")

  dir=kdecarddecks/${style}

  if [ -d ${dir} ]; then
    echo \"${dir}\" already exists
    shift
    continue
  fi



  echo "converting $1 to $dir"

  # copying the copyright

  mkdir -p ${dir}  

  cp $1/COPYRIGHT ${dir}/

  # converting the cards

  mkdir -p ${dir}/cards/club
  mkdir -p ${dir}/cards/spade
  mkdir -p ${dir}/cards/heart
  mkdir -p ${dir}/cards/diamond

  cp $1/1.png  ${dir}/cards/club/ace.png
  cp $1/17.png ${dir}/cards/club/ten.png
  cp $1/5.png  ${dir}/cards/club/king.png
  cp $1/9.png  ${dir}/cards/club/queen.png
  cp $1/13.png ${dir}/cards/club/jack.png
  cp $1/21.png ${dir}/cards/club/nine.png

  cp $1/2.png  ${dir}/cards/spade/ace.png
  cp $1/18.png ${dir}/cards/spade/ten.png
  cp $1/6.png  ${dir}/cards/spade/king.png
  cp $1/10.png ${dir}/cards/spade/queen.png
  cp $1/14.png ${dir}/cards/spade/jack.png
  cp $1/22.png ${dir}/cards/spade/nine.png

  cp $1/3.png  ${dir}/cards/heart/ace.png
  cp $1/19.png ${dir}/cards/heart/ten.png
  cp $1/7.png  ${dir}/cards/heart/king.png
  cp $1/11.png ${dir}/cards/heart/queen.png
  cp $1/15.png ${dir}/cards/heart/jack.png
  cp $1/23.png ${dir}/cards/heart/nine.png

  cp $1/4.png  ${dir}/cards/diamond/ace.png
  cp $1/20.png ${dir}/cards/diamond/ten.png
  cp $1/8.png  ${dir}/cards/diamond/king.png
  cp $1/12.png ${dir}/cards/diamond/queen.png
  cp $1/16.png ${dir}/cards/diamond/jack.png
  cp $1/24.png ${dir}/cards/diamond/nine.png

  # create the icons

  pushd . >/dev/null

  cd ${dir}
  mkdir -p icons
  cd icons/
  ${CREATE_ICONS}

  popd >/dev/null

  # converting the decks

  mkdir -p ${dir}/backs

  for f in $(dirname $1)/decks/*.png; do
    if [ ${width} = $(identify -format "%w" $f) \
      -a ${height} = $(identify -format "%h" $f) ]; then
      cp $f ${dir}/backs/$(basename $f)
    fi
  done

  shift
done


exit 0
