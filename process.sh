#!/bin/bash

print_usage() {
    echo "usage: script [OPTIONS] FILE" >&2
    echo "" >&2
    echo "options:" >&2
    echo "  -h            show this help message and exit" >&2
    echo "  -d DATASET    dataset to be clustered" >&2
    echo "  -k CLUSTERS   number of clusters" >&2
    echo "  -n SAMPLES    number of iterations to perform" >&2
    echo "  -m MULT       initial lower bound multiplier" >&2
}

# defaults
n=5
m=1.25

while getopts ":hd:k:n:m:" opt; do
    case $opt in
        h)
            print_usage
            exit 1
            ;;
        d)
            d=$OPTARG
            ;;
        k)
            k=$OPTARG
            ;;
        n)
            n=$OPTARG
            ;;
        m)
            m=$OPTARG
            ;;
        \?)
            echo "invalid option: -$OPTARG" >&2
            print_usage
            exit 1
            ;;
        :)
            echo "option -$OPTARG requires an argument." >&2
            print_usage
            exit 1
            ;;
    esac
done

if [[ -z $d ]]; then
    echo "missing dataset, stopping"
    exit 1
fi

dataset=$d
data_folder="datasets/$dataset"
data_file="$data_folder/$dataset.dat"

if [[ !(-f $data_file) ]]; then
    echo "no such file or directory: $data"
    exit 1
fi

if [[ -f $data_folder/processed.dat ]]; then
    rm $data_folder/processed.dat
fi

# afinidade com o processador 3(4)
cmd="taskset 8 ./kmeans $data_file -q"
if [[ !(-z $k) ]]; then
    cmd+=" -k $k"
fi

for (( i=1; i <= $n; i++));
do
    seed=$RANDOM
    yy=`$cmd -s $seed -a yy`
    yy_time=`echo $yy | cut -f1 -d' '`
    # yy_clst=`echo $yy | cut -f2 -d' '`
    yy_clst=`echo $yy | cut -f2 -d' ' | sed -e 's/./&,/g' -e '$ s/.$//'`

    yy_acc=`$cmd -s $seed -a yy -m $m`
    yy_acc_time=`echo $yy_acc | cut -f1 -d' '`
    # yy_acc_clst=`echo $yy_acc | cut -f2 -d' '`
    yy_acc_clst=`echo $yy_acc | cut -f2 -d' ' | sed -e 's/./&,/g' -e '$ s/.$//'`

    # arquivos temporarios a serem usados pelo Rscript
    echo "$yy_clst" > $data_folder/yy.dat
    echo "$yy_acc_clst" > $data_folder/yy_acc.dat

    gain=`echo "scale=6; 1 - ($yy_acc_time / $yy_time)" | bc`
    similarity=`Rscript randindex.R /dev/stdout\
                --args $data_folder/yy.dat $data_folder/yy_acc.dat`

    echo "$gain,$similarity,$m" >> $data_folder/processed.dat
done

rm $data_folder/yy.dat
rm $data_folder/yy_acc.dat
