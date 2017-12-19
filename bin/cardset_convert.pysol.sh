#!/bin/bash
#
# converts pysol-cardsets to FreeDoko-cardsets
# uses imagemagick
#
# ~/Programme/FreeDoko/data/bin/pysol-cardsets-to-FreeDoko-cardsets /usr/share/games/pysol/cardset-*
# 
# 18. June 2002
# Diether Knof

# copying the copyright

mkdir -p pysol
cp /usr/share/doc/pysol-cardsets/copyright pysol/

CREATE_ICONS="$(pwd)/create_icons"

while [ $# -gt 0 ]; do
  if [ ! -d $1 ]; then
    echo \"$1\" is not a directory
    shift
    continue
  fi

  style=$(basename $1 \ | sed "s/^cardset-//")

  if [ ! -f $1/13c.gif ]; then
    echo \"$1\" is not a pysol directory
    shift
    continue
  fi

  width=$(identify -format "%w" "$1/01c.gif")
  height=$(identify -format "%h" "$1/01c.gif")

  dir=pysol/${style}

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

  convert $1/01c.gif ${dir}/cards/club/ace.png
  convert $1/10c.gif ${dir}/cards/club/ten.png
  convert $1/13c.gif ${dir}/cards/club/king.png
  convert $1/12c.gif ${dir}/cards/club/queen.png
  convert $1/11c.gif ${dir}/cards/club/jack.png
  convert $1/09c.gif ${dir}/cards/club/nine.png

  convert $1/01s.gif ${dir}/cards/spade/ace.png
  convert $1/10s.gif ${dir}/cards/spade/ten.png
  convert $1/13s.gif ${dir}/cards/spade/king.png
  convert $1/12s.gif ${dir}/cards/spade/queen.png
  convert $1/11s.gif ${dir}/cards/spade/jack.png
  convert $1/09s.gif ${dir}/cards/spade/nine.png

  convert $1/01h.gif ${dir}/cards/heart/ace.png
  convert $1/10h.gif ${dir}/cards/heart/ten.png
  convert $1/13h.gif ${dir}/cards/heart/king.png
  convert $1/12h.gif ${dir}/cards/heart/queen.png
  convert $1/11h.gif ${dir}/cards/heart/jack.png
  convert $1/09h.gif ${dir}/cards/heart/nine.png

  convert $1/01d.gif ${dir}/cards/diamond/ace.png
  convert $1/10d.gif ${dir}/cards/diamond/ten.png
  convert $1/13d.gif ${dir}/cards/diamond/king.png
  convert $1/12d.gif ${dir}/cards/diamond/queen.png
  convert $1/11d.gif ${dir}/cards/diamond/jack.png
  convert $1/09d.gif ${dir}/cards/diamond/nine.png


  # create the icons

  pushd . >/dev/null

  cd ${dir}
  mkdir -p icons
  cd icons/
  ${CREATE_ICONS}

  popd >/dev/null

  # converting the decks

  mkdir -p ${dir}/backs

  for f in $1/back*.gif; do
    if [ ${width} = $(identify -format "%w" $f) \
      -a ${height} = $(identify -format "%h" $f) ]; then
      convert $f ${dir}/backs/$(basename $f .gif).png
    fi
  done

  shift
done


exit 0
