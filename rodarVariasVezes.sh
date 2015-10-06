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


MENOR=`./kmeans $SEP $DIR/cini $DIR/cfim $DIR/gid $K $Y < $DIR/$DATASET > $DIR/out | grep RSS | tail -1 | cut -d\  -f 3`;

for i in `seq 1 $ITERACOES`; do
  ATUAL=`./kmeans $SEP $DIR/cini $DIR/cfim $DIR/gid $K $Y < $DIR/$DATASET > $DIR/out | grep RSS | tail -1 | cut -d\  -f 3`;
  if [ $MENOR -gt $ATUAL ]; then
    MENOR=$ATUAL;
    echo $MENOR ;
  fi  ;
done
