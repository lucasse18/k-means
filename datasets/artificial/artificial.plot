
set term "png"

set output "artificial.png"


set xtics ".1"

set xrange [0:1]


plot "< sed 's/,/ /g' artificial.dat" title "minha figura favorita" with impulses


