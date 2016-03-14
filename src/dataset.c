#include "../include/dataset.h"

#define MAX_LINE_LENGTH BUFSIZ

void dataset_init(Dataset *data, FILE *data_file) {

  VECTOR_INIT(data->ex);

  int attribute_count = 0, last_attribute_count = -1;

  FILE *string_stream;
  float attribute_value;
  char linha[MAX_LINE_LENGTH];
  while (fgets(linha, MAX_LINE_LENGTH, data_file) != NULL) {
    if (linha[0] != '%') {
      if (last_attribute_count != -1 && last_attribute_count != attribute_count) {
        printf("ERROR: Datasets with missing attributes not supported.\n");
        printf("current attribute count: %d\n", attribute_count);
        printf("last attribute count: %d\n", last_attribute_count);
        exit(1);
      }
      attribute_count = 0;
      string_stream = fmemopen(linha, strlen(linha), "r");
      while (fscanf(string_stream, "%f", &attribute_value) == 1) {
        attribute_count++;
        fgetc(string_stream);
        VECTOR_APPEND(data->ex, attribute_value);
      }
      last_attribute_count = attribute_count;
      fclose(string_stream);
    }
  }
  if (feof(data_file) && data_file != stdin)
    fclose(data_file);
  else {
    printf("ERROR: fgets returned NULL before EOF.");
    exit(1);
  }

  data->nat = attribute_count;
  data->nex = VECTOR_SIZE(data->ex) / attribute_count;
}

inline void dataset_free(Dataset *data) {
  VECTOR_FREE(data->ex);
}
