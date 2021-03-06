#ifndef DATASET_H
#define DATASET_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vector.h"

#define DATASET_INIT(dataset, infile) dataset_init(&dataset, infile)
#define DATASET_FREE(dataset) dataset_free(&dataset);

typedef struct dataset {
  vector ex;
  size_t nex;
  size_t nat;
} Dataset;

void dataset_init(Dataset *data, FILE *data_file);
void dataset_free(Dataset *data);


#endif //DATASET_H
