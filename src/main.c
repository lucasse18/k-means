#include "utils.h"
#define BUFSIZE 1024

int main(int argc, char* argv[]) {

  if(argc < 7) {
    fprintf(stderr, "Argumentos insuficientes\n");
    exit(EXIT_FAILURE);
  }

  argv++;//descarta o nome do programa

  FILE *centrosIniciaisFile = fopen(argv[1], "w");
  if(centrosIniciaisFile == NULL) {
    fprintf(stderr, "Erro ao abrir arquivo %s", argv[1]);
    exit(EXIT_FAILURE);
  }

  FILE *centrosFile = fopen(argv[2], "w");
  if(centrosFile == NULL) {
    fprintf(stderr, "Erro ao abrir arquivo %s", argv[2]);
    exit(EXIT_FAILURE);
  }

  FILE *atribuicoesFile = fopen(argv[3], "w");
  if(atribuicoesFile == NULL) {
    fprintf(stderr, "Erro ao abrir arquivo %s", argv[3]);
    exit(EXIT_FAILURE);
  }

  unsigned nLinhas, nColunas;
  fscanf(stdin,"%d %d", &nLinhas, &nColunas);
  fgetc(stdin);//descarta quebra de linha

  unsigned K = (unsigned)atoi(argv[4]);

  /* INICIO ALOCACAO MEMORIA */
  unsigned *melhorGrupo = calloc(nLinhas, sizeof(unsigned));
  if(melhorGrupo == NULL) {
    fprintf(stderr, "Erro de alocacao.\n");
    exit(EXIT_FAILURE);
  }

  unsigned *segMelhorGrupo = calloc(nLinhas, sizeof(unsigned));
  if(segMelhorGrupo == NULL) {
    fprintf(stderr, "Erro de alocacao.\n");
    exit(EXIT_FAILURE);
  }

  char *aux = calloc(BUFSIZE, sizeof(char));
  if(aux == NULL) {
    fprintf(stderr, "Erro de alocacao.\n");
    exit(EXIT_FAILURE);
  }

  float **centros = (float **)calloc(K, sizeof(float *));
  if(centros == NULL) {
    fprintf(stderr, "Erro de alocacao.\n");
    exit(EXIT_FAILURE);
  }
  for(unsigned i = 0; i < K; i++) {
    centros[i] = (float *)calloc(nColunas, sizeof(float));
    if(centros[i] == NULL) {
      fprintf(stderr, "Erro de alocacao.\n");
      exit(EXIT_FAILURE);
    }
  }

  float **centrosAnt = (float **)calloc(K, sizeof(float *));
  if(centrosAnt == NULL) {
    fprintf(stderr, "Erro de alocacao.\n");
    exit(EXIT_FAILURE);
  }
  for(unsigned i = 0; i < K; i++) {
    centrosAnt[i] = (float *)calloc(nColunas, sizeof(float));
    if(centrosAnt[i] == NULL) {
      fprintf(stderr, "Erro de alocacao.\n");
      exit(EXIT_FAILURE);
    }
  }

  float **exemplos = (float **)calloc(nLinhas, sizeof(float *));
  if(exemplos == NULL) {
    fprintf(stderr, "Erro de alocacao.\n");
    exit(EXIT_FAILURE);
  }
  for(unsigned i = 0; i < nLinhas; i++) {
    exemplos[i] = (float *)calloc(nColunas, sizeof(float));
    if(exemplos[i] == NULL) {
      fprintf(stderr, "Erro de alocacao.\n");
      exit(EXIT_FAILURE);
    }
  }

  float *upperBound = (float *)calloc(nLinhas, sizeof(float));
  if(upperBound == NULL) {
    fprintf(stderr, "Erro de alocacao.\n");
    exit(EXIT_FAILURE);
  }

  float *lowerBound = (float *)calloc(nLinhas, sizeof(float));
  if(lowerBound == NULL) {
    fprintf(stderr, "Erro de alocacao.\n");
    exit(EXIT_FAILURE);
  }

  float *variacao = (float *)calloc(nLinhas, sizeof(float));
  if(variacao == NULL) {
    fprintf(stderr, "Erro de alocacao.\n");
    exit(EXIT_FAILURE);
  }

  char **grupoVerdadeiro = (char **)calloc(nLinhas, sizeof(char *));
  if(grupoVerdadeiro == NULL) {
    fprintf(stderr, "Erro de alocacao.\n");
    exit(EXIT_FAILURE);
  }
  for(unsigned i = 0; i < nLinhas; i++) {
    grupoVerdadeiro[i] = (char *)calloc(BUFSIZE, sizeof(char));
    if(grupoVerdadeiro[i] == NULL) {
      fprintf(stderr, "Erro de alocacao.\n");
      exit(EXIT_FAILURE);
    }
  }
  /* FIM ALOCACAO MEMORIA */

  /* INÍCIO LEITURA DADOS */
  unsigned tamSeparador = (unsigned)strlen(argv[0]);
  for(unsigned i = 0; i < nLinhas; i++) {
    if(olhaChar() != '%') {
      for(unsigned j = 0; j < nColunas; j++) {
        fscanf(stdin, "%f", &exemplos[i][j]);
        //fprintf(stdout, "%g%s", exemplos[i][j], argv[0]);//teste
        for(unsigned k = 0; k < tamSeparador; k++)
          fgetc(stdin);//retira separador
      }
      if(argv[5][0] == 'y') {
        //retira e guarda coluna de classe se argv[5][0] for igual a y
        fgets(grupoVerdadeiro[i], BUFSIZE, stdin);
        //fprintf(stdout, "%s", grupoVerdadeiro[i]);//teste
      }
      else {
        //retira e ignora coluna de classe se argv[5][0] nao for igual a y
        fgets(aux, BUFSIZE, stdin);
        //fputc('\n', stdout);//teste
      }
    }
    else {
      fgets(aux, BUFSIZE, stdin);
      i--;
    }
  }
  /* FIM LEITURA DADOS */

  //TODO repetir K-MEANS n vezes e extrair melhor RSS
  /* INÍCIO K-MEANS */
  //gera os índices dos elementos usados na criação dos grupos iniciais
  int index;
  int *gerados = calloc(K, sizeof(int));
  struct timespec tempo;
  if (clock_gettime(CLOCK_REALTIME, &tempo) == -1) {
    perror("clock gettime");
    exit(EXIT_FAILURE);
  }
  __time_t seed =  tempo.tv_sec*tempo.tv_nsec;
  srand((unsigned int) seed);

  for(unsigned i = 0; i < K; i++) {
    index = rand()%nLinhas;
    if(pertence(index, gerados, i))
      i--;
    else
      gerados[i] = index;
  }

  //inicializa centros aleatorios
  for(unsigned i = 0; i < K; i++) {
    for(unsigned j = 0; j < nColunas; j++) {
      centros[i][j] = exemplos[gerados[i]][j];
      centrosAnt[i][j] = centros[i][j];
      fprintf(centrosIniciaisFile, "%f", centros[i][j]);
      fprintf(centrosIniciaisFile, "%s", argv[0]);
    }
    fprintf(centrosIniciaisFile, " Exemplo %d\n", gerados[i]);
  }
  fclose(centrosIniciaisFile);

  unsigned melhor = 0, trocas = 0;
  float menorDistancia, dAtual, RSS = 0;
  float *acumulador = calloc(K, sizeof(float));//zerado
  int *qtdExemplosGrupo = calloc(K, sizeof(int));//zerado

  //atribui
  for(unsigned i = 0; i < nLinhas; i++) {
    menorDistancia = distancia(exemplos[i], centros[0], nColunas);
    melhor = 0;
    for(unsigned j = 1; j < K; j++) {
      dAtual = distancia(exemplos[i], centros[j], nColunas);
      if(dAtual < menorDistancia) {
        lowerBound[i] = menorDistancia;
        menorDistancia = dAtual;
        upperBound[i] = menorDistancia;
        melhor = j;
      }
    }
    if(melhorGrupo[i] != melhor) {
      trocas++;
      segMelhorGrupo[i] = melhorGrupo[i];
      melhorGrupo[i] = melhor;
    }
    qtdExemplosGrupo[melhor]++;
  }

  //calcula variacao
  for(unsigned i = 0; i < K; i++) {
    variacao[i] = distancia(centros[i], centrosAnt[i], K);
    fprintf(stderr, "Variacao[%d]: %f\n", i, variacao[i]);
  }

  //calcula qualidade do agrupamento
  for(unsigned i = 0; i < nLinhas; i++)
    RSS += distancia(exemplos[i], centros[melhorGrupo[i]], nColunas);
  fprintf(stderr, "RSS: %f\n", RSS);

  while(1) {
    //verifica se houve mudança
    if(!trocas)
      break;

    //recomputa
    for(unsigned i = 0; i < nColunas; i++) {
      //zera acumuladores antes da proxima iteração
      for(unsigned j = 0; j < K; j++)
        acumulador[j] = 0;
      //soma o valor da coordenada i dos exemplos em seu respectivo acumulador
      for(unsigned j = 0; j < nLinhas; j++)
        acumulador[melhorGrupo[j]] += exemplos[j][i];
      //atualiza o centro com a media dos pontos
      for(unsigned j = 0; j < K; j++) {
        centrosAnt[j][i] = centros[j][i];//salva centro anterior
        centros[j][i] = ((acumulador[j])/qtdExemplosGrupo[j]);
      }
    }

    for(unsigned i = 0; i < nLinhas; i++) {
      upperBound[i] = distancia(exemplos[i], centros[melhorGrupo[i]], nColunas);
      lowerBound[i] =distancia(exemplos[i], centros[segMelhorGrupo[i]], nColunas);
    }

    //calcula variacao dos centros
    for(unsigned i = 0; i < K; i++) {
      variacao[i] = distancia(centros[i], centrosAnt[i], K);
      fprintf(stderr, "Variacao[%d]: %f\n", i, variacao[i]);
    }

    //zera contadores
    RSS = 0; trocas = 0;
    for(unsigned i = 0; i < K; i++)
      qtdExemplosGrupo[i] = 0;

    //atribui
    for(unsigned i = 0; i < nLinhas; i++) {
      //FIXME condicao
//      if(lowerBound[i] - max(variacao, nLinhas) >= (upperBound[i] + distancia(
//          centros[melhorGrupo[i]], centrosAnt[melhorGrupo[i]], nColunas))) {
        menorDistancia = distancia(exemplos[i], centros[0], nColunas);
        melhor = 0;
        for(unsigned j = 1; j < K; j++) {
          dAtual = distancia(exemplos[i], centros[j], nColunas);
          if(dAtual < menorDistancia) {
            menorDistancia = dAtual;
            melhor = j;
          }
        }
        if(melhorGrupo[i] != melhor) {
          trocas++;
          segMelhorGrupo[i] = melhorGrupo[i];
          melhorGrupo[i] = melhor;
        }
        qtdExemplosGrupo[melhor]++;
//      }
    }

    //calcula qualidade do agrupamento
    for(unsigned i = 0; i < nLinhas; i++)
      RSS += distancia(exemplos[i], centros[melhorGrupo[i]], nColunas);
    fprintf(stderr, "RSS: %f\n", RSS);
  }
  /* FIM K-MEANS */
  //TODO repetir K-MEANS n vezes e extrair melhor RSS

  //salva centros finais
  for(unsigned i = 0; i < K; i++) {
    for(unsigned j = 0; j < nColunas; j++) {
      fprintf(centrosFile, "%f", centros[i][j]);
      if(i != K -1)
        fprintf(centrosFile, "%s", argv[0]);
    }
    fputc('\n', centrosFile);
  }

  //salva atribuicoes
  for(unsigned i = 0; i < nLinhas; i++)
    fprintf(atribuicoesFile, "%d\n", melhorGrupo[i]);

  fclose(centrosFile);
  fclose(atribuicoesFile);

  for(unsigned i = 0; i < K; i++)
    fprintf(stderr, "%d exemplos no grupo %d\n", qtdExemplosGrupo[i], i);

  if(argv[5][0] == 'y') {
    for(unsigned i = 0; i < nLinhas; i++)
      fprintf(stdout, "Escolhido: %d, Verdadeiro: %s", melhorGrupo[i], grupoVerdadeiro[i]);
  }

  /* INÍCIO DESALOCAÇÃO DE MEMÓRIA */
  free(qtdExemplosGrupo);
  free(acumulador);
  free(gerados);
  free(melhorGrupo);
  free(upperBound);
  free(lowerBound);
  free(variacao);
  free(aux);

  for(unsigned i = 0; i < K; i++)
    free(centrosAnt[i]);
  free(centrosAnt);

  for(unsigned i = 0; i < K; i++)
    free(centros[i]);
  free(centros);

  for(unsigned i = 0; i < nLinhas; i++)
    free(exemplos[i]);
  free(exemplos);

  for(unsigned i = 0; i < nLinhas; i++)
    free(grupoVerdadeiro[i]);
  free(grupoVerdadeiro);
  /* FIM DESALOCAÇÃO DE MEMÓRIA */

  return 0;
}
