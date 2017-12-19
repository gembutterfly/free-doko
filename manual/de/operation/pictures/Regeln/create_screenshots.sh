#!/bin/sh

# Creates the files for the manual from the screenshot files 
# uses imagemagick

#mv ~/"screenshot.FreeDoko Regeln: Turnier.png"       tournament.png
#mv ~/"screenshot.FreeDoko Regeln: Zählung.png"       counting.png
#mv ~/"screenshot.FreeDoko Regeln: Informationen.png" informations.png
#mv ~/"screenshot.FreeDoko Regeln: Solo.png"          solo.png
#mv ~/"screenshot.FreeDoko Regeln: Armut.png"         poverty.png
#mv ~/"screenshot.FreeDoko Regeln: Hochzeit.png"      marriage.png
#mv ~/"screenshot.FreeDoko Regeln: Ansagen.png"       announcements.png
#mv ~/"screenshot.FreeDoko Regeln: Dollen.png"        dollen.png
#mv ~/"screenshot.FreeDoko Regeln: Schweine.png"      swines.png
#mv ~/"screenshot.FreeDoko Regeln: Sonderpunkte.png"  specialpoints.png

rm *.small.png
for f in *.png; do
  convert -scale 50% $f `basename $f .png`.small.png
done
