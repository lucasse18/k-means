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

stats "$folder/$dataset/$filename" using 1 name "gain"
stats "$folder/$dataset/$filename" using 2 name "quality"
stats "$folder/$dataset/$filename" using 3 name "mult"

# primeiro grafico, similaridade x mult
set output "$dataset-quality_mult.png"
set xlabel "Mult"
set ylabel "RIndex"

plot "$folder/$dataset/$filename" u 3:2 t "iteration", quality_mean t "RIndex mean"

# segundo grafico, ganho x mult
set output "$dataset-gain_mult.png"
set xlabel "Mult"
set ylabel "Gain(%)"

plot "$folder/$dataset/$filename" u 3:1 t "iteration", gain_mean t "gain mean"

# terceiro grafico, ganho x similaridade
set output "$dataset-gain_quality.png"
set xlabel "RIndex"
set ylabel "Gain(%)"

# FIXME RIndex mean deveria estar no eixo x
plot "$folder/$dataset/$filename" u 2:1 t "iteration", gain_mean t "gain mean", quality_mean t "RIndex mean"

# # quarto grafico, ganho x similaridade x mult
# set output "$dataset-gain_quality_mult.png"
# set xlabel "Mult"
# set ylabel "RIndex"
# set zlabel "Gain (%)"

# splot "$folder/$dataset/$filename" u 3:2:1 t "iteration"
EOF
