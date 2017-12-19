#!/bin/zsh

if [ ! $# -eq 2 ]; then
  echo "$#  $@"
  echo "Zwei Argumente (Breite und Höhe) übergeben."
  exit
fi
GROESSE=$1x$2

pushd $(dirname "$0") >/dev/null

xaos -loadpos fract0.xpf -size $GROESSE -saveimg xaos.png 2>/dev/null &
PID=$!
sleep 1 
kill $PID

popd >/dev/null
