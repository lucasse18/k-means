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

folder="datasets"

gnuplot<<EOF
set datafile separator ","
set title "$dataset-$filename"
set term "png"

stats "$folder/$dataset/$filename" using 1 name "time"
stats "$folder/$dataset/$filename" using 2 name "quality"
stats "$folder/$dataset/$filename" using 3 name "mult"

# primeiro grafico, similaridade x mult
set output "$dataset-quality_mult.png"
set xlabel "Mult"
set ylabel "RIndex"

plot "$folder/$dataset/$filename" using 3:2 t "dados", quality_mean t "média Rindex"

# segundo grafico, tempo x mult
set output "$dataset-time_mult.png"
set xlabel "Mult"
set ylabel "Ganho (%)"

plot "$folder/$dataset/$filename" using 3:1 t "dados", time_mean t "média tempo"

# terceiro grafico, similaridade x ganho x mult
set output "$dataset-quality_time_mult.png"
set xlabel "Mult"
set ylabel "RIndex"
set zlabel "Ganho (%)"

splot "$folder/$dataset/$filename" using 3:1:2 t "dados", time_mean t "média tempo", quality_mean t "média Rindex"
EOF
