#include "utils.h"

int main(int argc, char* argv[]) {

  if(argc < 5) {
    fprintf(stdout, "Argumentos insuficientes");
    exit(EXIT_FAILURE);
  }

  argv++;//descarta o nome do programa

  FILE *centrosIniciaisFile = fopen(argv[1], "w");
  if(centrosIniciaisFile == NULL) {
    fprintf(stdout, "Erro ao abrir arquivo %s", argv[1]);
    exit(EXIT_FAILURE);
  }

  FILE *centrosFile = fopen(argv[2], "w");
  if(centrosFile == NULL) {
    fprintf(stdout, "Erro ao abrir arquivo %s", argv[2]);
    exit(EXIT_FAILURE);
  }

  FILE *atribuicoesFile = fopen(argv[3], "w");
  if(atribuicoesFile == NULL) {
    fprintf(stdout, "Erro ao abrir arquivo %s", argv[3]);
    exit(EXIT_FAILURE);
  }

  int nLinhas, nColunas;
  fscanf(stdin,"%d %d", &nLinhas, &nColunas);
  fgetc(stdin);//descarta quebra de linha

  int K = (int)sqrt((double)nLinhas);

  int *gID = calloc(nLinhas, sizeof(int));

  double **centros = (double **)calloc(K, sizeof(double *));
  for(int i = 0; i < K; i++)
    centros[i] = (double *)calloc(nColunas, sizeof(double));

  double **exemplos = (double **)calloc(nLinhas, sizeof(double *));
  for(int i = 0; i < nLinhas; i++)
    exemplos[i] = (double *)calloc(nColunas, sizeof(double));

  char **grupoVerdadeiro = (char **)calloc(nLinhas, sizeof(char *));
  for(int i = 0; i < nLinhas; i++)//FIXME alocação não dinâmica de strings
    grupoVerdadeiro[i] = (char *)calloc(17, sizeof(char));

  /* INÍCIO LEITURA DADOS */
  int tamSeparador = strlen(argv[0]);
  for(int i = 0; i < nLinhas; i++) {
    for(int j = 0; j < nColunas; j++) {
      fscanf(stdin, "%lf", &exemplos[i][j]);
      fprintf(stdout, "%.1f", exemplos[i][j]);//teste
      for(int k = 0; k < tamSeparador; k++)
        //fgetc(stdin);//descarta separadores
        fputc(fgetc(stdin), stdout);//teste
    }
    fgets(grupoVerdadeiro[i], 17, stdin);//contém quebra de linha
    fprintf(stdout, "%s", grupoVerdadeiro[i]);//teste
  }
  /* FIM LEITURA DADOS */

  //gera os índices dos elementos usados na criação dos grupos iniciais
  int index, gerados[K];
  srand((unsigned)time(NULL));
  for(int i = 0; i < K; i++) {
    index = rand()%K;
    if(pertence(index, gerados, i))
      i--;
    else
      gerados[i] = index;
  }

  /* INÍCIO K-MEANS */
  //cria grupos iniciais
  for(int i = 0; i < K; i++) {
    for(int j = 0; j < nColunas; j++) {
      centros[i][j] = exemplos[gerados[i]][j];
      fprintf(centrosIniciaisFile, "%lf", centros[i][j]);
      fprintf(centrosIniciaisFile, "%s", argv[0]);//FIXME separador sobrando no fim
    }
    fputc('\n', centrosIniciaisFile);
  }
  fclose(centrosIniciaisFile);

  int melhorGrupo, troca;
  double menorDistanca, dAtual;

  do {
    //para cada ponto, encontra qual o centro mais próximo
    troca = 0;
    for(int i = 0; i < nLinhas; i++) {
      menorDistanca = distancia(exemplos[i], centros[0], nColunas);
      for(int j = 1; j < K; j++) {
        dAtual = distancia(exemplos[i], centros[j], nColunas);
        //dAtual = 0 significa que o exemplo coincidiu com o centro
        if((dAtual != 0) && (dAtual < menorDistanca)) {
          menorDistanca = dAtual;
          melhorGrupo = j;
        }
      }
      if(gID[i] != melhorGrupo) {
        troca++;
        gID[i] = melhorGrupo;
      }
    }

    //FIXME adicionar condicao de parada no while
    //verifica se houve mudança
    if(!troca)
      break;

    //recomputa os centros depois de atribuídos os exemplos
    for(int i = 0; i < nLinhas; i++) {
      for(int j = 0; j < nColunas; j++) {
        if(centros[gID[i]][j] < exemplos[i][j]) {
          centros[gID[i]][j] = (exemplos[i][j] - centros[gID[i]][j])/2;
        }
        else {
          centros[gID[i]][j] = (centros[gID[i]][j] - exemplos[i][j])/2;
        }
      }
    }
  }while(1);
  /* FIM K-MEANS */

  //salva centros finais
  for(int i = 0; i < K; i++) {
    for(int j = 0; j < nColunas; j++) {
      fprintf(centrosFile, "%lf", centros[i][j]);
      fprintf(centrosFile, "%s", argv[0]);//FIXME separador sobrando no fim
    }
    fputc('\n', centrosFile);
  }

  //salva atribuicoes
  for(int i = 0; i < nLinhas; i++) {
    fprintf(atribuicoesFile, "%d\n", gID[i]);
  }

  fclose(centrosFile);
  fclose(atribuicoesFile);

  /* INÍCIO DESALOCAÇÃO DE MEMÓRIA */
  free(gID);

  for(int i = 0; i < K; i++)
    free(centros[i]);
  free(centros);

  for(int i = 0; i < nLinhas; i++)
    free(exemplos[i]);
  free(exemplos);

  for(int i = 0; i < nLinhas; i++)
    free(grupoVerdadeiro[i]);
  free(grupoVerdadeiro);
  /* FIM DESALOCAÇÃO DE MEMÓRIA */

  return 0;
}
