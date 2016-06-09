dataset=$1
if [[ -z $dataset ]]
then
  echo "missing dataset, stopping."
  exit
fi

filename=$2
if [[ -z $filename ]]
then
  filename="processed.dat"
fi

folder="../datasets"

gnuplot<<EOF
set datafile separator ","
set title "$dataset-$filename"
set term "png"

stats "$folder/$dataset/$filename" using 1 name "gain"
stats "$folder/$dataset/$filename" using 2 name "error"
stats "$folder/$dataset/$filename" using 3 name "mult"

# primeiro grafico, erros x mult
set output "$dataset-$filename-error_x_mult.png"
set xlabel "multiplicador"
set ylabel "erros produzidos"

plot "$folder/$dataset/$filename" using 3:2 t "dados", error_mean t "media erros"

# segundo grafico, ganho x mult
set output "$dataset-$filename-gain_x_mult.png"
set xlabel "multiplicador"
set ylabel "calculos evitados (%)"

plot "$folder/$dataset/$filename" using 3:1 t "dados", gain_mean t "media ganho"

# terceiro grafico, erros x ganho x mult
set output "$dataset-$filename-error_x_gain_x_mult.png"
set xlabel "multiplicador"
set ylabel "erros produzidos"
set zlabel "calculos evitados (%)"

splot "$folder/$dataset/$filename" using 3:1:2 t "dados", gain_mean t "media ganho", error_mean t "media erros"
EOF
