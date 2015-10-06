#!/bin/bash

#$1 = separador
#$2 = diretorio
#$3 = nome dataset
#$4 = numero de grupos
#$5 = presenca de coluna de classe
#$6 = numero de execucoes

MENOR=`./kmeans $1 $2/cini $2/cfim $2/gid $4 $5 < $2/$3 > $2/out | grep RSS | tail -1 | cut -d\  -f 3`;

for i in `seq 1 $6`; do
  ATUAL=`./kmeans $1 $2/cini $2/cfim $2/gid $4 $5 < $2/$3 > $2/out | grep RSS | tail -1 | cut -d\  -f 3`;
  if [ $MENOR -gt $ATUAL ]; then
    MENOR=$ATUAL;
    echo $MENOR ;
  fi  ;
done
