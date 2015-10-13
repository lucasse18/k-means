#!/bin/bash

#$1 = separador
#$2 = diretorio
#$3 = nome dataset
#$4 = numero de grupos
#$5 = presenca de coluna de classe
#$6 = numero de execucoes

SEP=$1
DIR=$2
DATASET=$3
K=$4
Y=$5
ITERACOES=$6

MENOR=`./kmeans $SEP $DIR $K $Y < $DIR/$DATASET 2>&1 > $DIR/out | grep RSS | tail -1 | cut -d " " -f 2`;
echo $MENOR;

for i in `seq 1 $ITERACOES`; do
  ATUAL=`./kmeans $SEP $DIR $K $Y < $DIR/$DATASET 2>&1 > $DIR/out | grep RSS | tail -1 | cut -d " " -f 2`;
  if [ `echo $MENOR '>' $ATUAL | bc -l` -eq 1 ]; then
    MENOR=$ATUAL;
    echo $MENOR;
  fi;
done

