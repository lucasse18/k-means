#include "dataset.h"
#include "dbg.h"

#define MAX_LINE_LENGTH BUFSIZ

void dataset_init(Dataset *data, FILE *data_file) {

  VECTOR_INIT(data->ex);

  size_t attribute_count = 0, last_attribute_count = 0;

  FILE *string_stream;
  float attribute_value;
  char linha[MAX_LINE_LENGTH];
  while (fgets(linha, MAX_LINE_LENGTH, data_file) != NULL) {
    if (linha[0] != '%') {
      if (last_attribute_count != 0 && last_attribute_count != attribute_count) {
        log_err("datasets with missing attributes not supported.\n\
                    current attribute count: %zd\n\
                    last attribute count: %zd",\
                     attribute_count, last_attribute_count);
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
  if (!feof(data_file)) {
    log_err("fgets returned NULL before EOF.");
    exit(1);
  }

  data->nat = attribute_count;
  data->nex = VECTOR_SIZE(data->ex) / attribute_count;
}

inline void dataset_free(Dataset *data) {
  VECTOR_FREE(data->ex);
}
