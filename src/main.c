#include "utils.h"
#define MY_BUFSIZ 512

int main(int argc, char* argv[]) {

  if(argc < 4) {
    fprintf(stderr, "Argumentos insuficientes\n");
    exit(EXIT_FAILURE);
  }

  argv++;//descarta o nome do programa

  char *nomeDoArquivo = calloc(MY_BUFSIZ, sizeof(char));

  sprintf(nomeDoArquivo, "%s/cini", argv[1]);
  FILE *centrosIniciais = fopen(nomeDoArquivo, "w");
  if(centrosIniciais == NULL) {
    fprintf(stderr, "Erro ao abrir arquivo %s", nomeDoArquivo);
    exit(EXIT_FAILURE);
  }

  sprintf(nomeDoArquivo, "%s/cfim", argv[1]);
  FILE *centrosFile = fopen(nomeDoArquivo, "w");
  if(centrosFile == NULL) {
    fprintf(stderr, "Erro ao abrir arquivo %s", nomeDoArquivo);
    exit(EXIT_FAILURE);
  }

  sprintf(nomeDoArquivo, "%s/grupos", argv[1]);
  FILE *atribuicoesFile = fopen(nomeDoArquivo, "w");
  if(atribuicoesFile == NULL) {
    fprintf(stderr, "Erro ao abrir arquivo %s", nomeDoArquivo);
    exit(EXIT_FAILURE);
  }

  unsigned nLinhas, nColunas;
  fscanf(stdin,"%d %d", &nLinhas, &nColunas);
  fgetc(stdin);//descarta quebra de linha

  unsigned K = (unsigned)atoi(argv[2]);

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

  char *comentario = calloc(BUFSIZ, sizeof(char));
  if(comentario == NULL) {
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

  float *variacao = (float *)calloc(K, sizeof(float));
  if(variacao == NULL) {
    fprintf(stderr, "Erro de alocacao.\n");
    exit(EXIT_FAILURE);
  }

  char **grupoVerdadeiro = (char **) calloc(nLinhas, sizeof(char *));
  if(grupoVerdadeiro == NULL) {
    fprintf(stderr, "Erro de alocacao.\n");
    exit(EXIT_FAILURE);
  }
  for(unsigned i = 0; i < nLinhas; i++) {
    grupoVerdadeiro[i] = (char *) calloc(MY_BUFSIZ, sizeof(char));
    if(grupoVerdadeiro[i] == NULL) {
      fprintf(stderr, "Erro de alocacao.\n");
      exit(EXIT_FAILURE);
    }
  }

  /* LEITURA DADOS */
  unsigned tamSeparador = (unsigned)strlen(argv[0]);
  for(unsigned i = 0; i < nLinhas; i++) {
    if(olhaChar() != '%') {
      for(unsigned j = 0; j < nColunas; j++) {
        fscanf(stdin, "%f", &exemplos[i][j]);
        //fprintf(stdout, "%g%s", exemplos[i][j], argv[0]);//teste
        for(unsigned k = 0; k < tamSeparador; k++)
          fgetc(stdin);//retira separador
      }
      //retira e guarda coluna de classe se argv[3][0] for igual a y
      if(argv[3][0] == 'y') {
        fgets(grupoVerdadeiro[i], MY_BUFSIZ, stdin);
      }
      //retira e ignora coluna de classe se argv[3][0] nao for igual a y
      else {
        fgets(comentario, MY_BUFSIZ, stdin);
      }
    }
    else {
      fgets(comentario, MY_BUFSIZ, stdin);
      i--;
    }
  }

  //TODO repetir K-MEANS n vezes e extrair melhor RSS
  /* K-MEANS */
  //gera os índices dos elementos usados na criação dos grupos iniciais
  srand48((unsigned int) seed());
  long *gerados = gera(K, nLinhas);

  //FIXME menos parametros
  //inicializa centros aleatorios
  InicializaCentros(&centros, &exemplos, gerados, K, nColunas, centrosIniciais, argv[0]);
  fclose(centrosIniciais);
  free(gerados);

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
  }

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
      lowerBound[i] = distancia(exemplos[i], centros[segMelhorGrupo[i]], nColunas);
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
      if(lowerBound[i] - max(variacao, K) >= (upperBound[i] + distancia(
          centros[melhorGrupo[i]], centrosAnt[melhorGrupo[i]], nColunas))) {
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
      }
      else {
        qtdExemplosGrupo[melhorGrupo[i]]++;
      }
    }

    //calcula qualidade do agrupamento
    for(unsigned i = 0; i < nLinhas; i++)
      RSS += upperBound[i];
    fprintf(stderr, "RSS: %f\n", RSS);
  }
  //TODO repetir K-MEANS n vezes e extrair melhor RSS

  /* SALVA RESULTADOS */
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

  if(argv[3][0] == 'y') {
    for(unsigned i = 0; i < nLinhas; i++)
      fprintf(stdout, "Escolhido: %d, Verdadeiro: %s", melhorGrupo[i], grupoVerdadeiro[i]);
  }

  free(qtdExemplosGrupo);
  free(acumulador);
  free(melhorGrupo);
  free(upperBound);
  free(lowerBound);
  free(variacao);
  free(comentario);

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

  return 0;
}
