#include "utils.h"

int main(int argc, char* argv[]) {

  if(argc < 6) {
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

  //TODO checar se houve erro de alocacao
  unsigned nLinhas, nColunas;
  fscanf(stdin,"%d %d", &nLinhas, &nColunas);
  fgetc(stdin);//descarta quebra de linha

  unsigned K = (unsigned)atoi(argv[4]);

  unsigned *gID = calloc(nLinhas, sizeof(unsigned));

  float **centros = (float **)calloc(K, sizeof(float *));
  for(unsigned i = 0; i < K; i++)
    centros[i] = (float *)calloc(nColunas, sizeof(float));

  float **exemplos = (float **)calloc(nLinhas, sizeof(float *));
  for(unsigned i = 0; i < nLinhas; i++)
    exemplos[i] = (float *)calloc(nColunas, sizeof(float));

  char **grupoVerdadeiro = (char **)calloc(nLinhas, sizeof(char *));
  for(unsigned i = 0; i < nLinhas; i++)//FIXME alocação não dinâmica de strings
    grupoVerdadeiro[i] = (char *)calloc(BUFSIZ, sizeof(char));

  /* INÍCIO LEITURA DADOS */
  unsigned  posatr;
  unsigned tamSeparador = (unsigned)strlen(argv[0]);
  char *aux = (char *)calloc(BUFSIZ, sizeof(char));
  char *atributo = (char *)calloc(64, sizeof(char));
  for(unsigned i = 0, posaux = 0; i < nLinhas; i++, posaux = 0) {
    fgets(aux, BUFSIZ, stdin);
    if(aux[0] != '%') {
      for(unsigned j = 0; j < nColunas; j++) {
        posatr = 0;
        while(aux[posaux] != argv[0][0]) {
          //TODO checar tamanho maximo pos
          atributo[posatr] = aux[posaux];
          posaux++; posatr++;
        }
        atributo[posatr] = '\0';
        exemplos[i][j] = (float)atof(atributo);
        //fprintf(stdout, "%g%s", exemplos[i][j], argv[0]);//teste
        for(unsigned k = 0; k < tamSeparador; k++)
          posaux++;//ignora separador
      }
      posatr = 0;
      while(aux[posaux] != '\n') {
        atributo[posatr] = aux[posaux];
        posaux++; posatr++;
      }
      atributo[posatr] = '\n';
      atributo[posatr+1] = '\0';
      strcpy(grupoVerdadeiro[i], atributo);//TODO checar erro
      //fprintf(stdout, "%s", grupoVerdadeiro[i]);//teste
    }
    else
      i--;
  }
  /* FIM LEITURA DADOS */

  //gera os índices dos elementos usados na criação dos grupos iniciais
  int index;
  int *gerados = calloc(K, sizeof(int));
  srand((unsigned)time(NULL));
  for(unsigned i = 0; i < K; i++) {
    index = rand()%K;
    if(pertence(index, gerados, i))
      i--;
    else
      gerados[i] = index;
  }

  /* INÍCIO K-MEANS */
  //cria grupos iniciais
  for(unsigned i = 0; i < K; i++) {
    for(unsigned j = 0; j < nColunas; j++) {
      centros[i][j] = exemplos[gerados[i]][j];
      fprintf(centrosIniciaisFile, "%lf", centros[i][j]);
      fprintf(centrosIniciaisFile, "%s", argv[0]);
    }
    fprintf(centrosIniciaisFile, " Exemplo %d\n", gerados[i]);
  }
  fclose(centrosIniciaisFile);

  unsigned melhorGrupo, troca;
  float menorDistanca, dAtual;
  float *acumulador = calloc(K, sizeof(float));
  int *qtdExemplosGrupo = calloc(K, sizeof(int));

  do {
    for(unsigned i = 0; i < K; i++)
      qtdExemplosGrupo[i] = 0;
    troca = 0;

    //para cada ponto, encontra qual o centro mais próximo
    for(unsigned i = 0; i < nLinhas; i++) {
      menorDistanca = distancia(exemplos[i], centros[0], nColunas);
      melhorGrupo = 0;
      for(unsigned j = 1; j < K; j++) {
        dAtual = distancia(exemplos[i], centros[j], nColunas);
        if(dAtual < menorDistanca) {
          menorDistanca = dAtual;
          melhorGrupo = j;
        }
      }
      if(gID[i] != melhorGrupo) {
        troca++;
        gID[i] = melhorGrupo;
      }
      qtdExemplosGrupo[melhorGrupo]++;
    }

    //FIXME condicao de parada while
    //verifica se houve mudança
    fprintf(stderr, "Trocou:%d ", troca);
    if(!troca) {
      fputc('\n', stderr);
      break;
    }

    //recomputa os centros depois de atribuídos os exemplos
    for(unsigned i = 0; i < nColunas; i++) {
      //zera acumuladores antes da proxima iteração
      for(unsigned j = 0; j < K; j++)
        acumulador[j] = 0;
      //soma o valor da coordenada i dos exemplos em seu respectivo acumulador
      for(unsigned j = 0; j < nLinhas; j++)
        acumulador[gID[j]] += exemplos[j][i];
      //atualiza o centro com a media dos pontos
      for(unsigned j = 0; j < K; j++)
        centros[j][i] = ((acumulador[j])/qtdExemplosGrupo[j]);
    }
  }while(1);
  /* FIM K-MEANS */

  //salva centros finais
  for(unsigned i = 0; i < K; i++) {
    for(unsigned j = 0; j < nColunas; j++) {
      fprintf(centrosFile, "%lf", centros[i][j]);
      fprintf(centrosFile, "%s", argv[0]);//FIXME separador sobrando no fim
    }
    fputc('\n', centrosFile);
  }

  //salva atribuicoes
  for(unsigned i = 0; i < nLinhas; i++) {
    fprintf(atribuicoesFile, "%d\n", gID[i]);
  }

  fclose(centrosFile);
  fclose(atribuicoesFile);

  for(unsigned i = 0; i < K; i++) {
    fprintf(stderr, "%d exeplos no grupo %d\n", qtdExemplosGrupo[i], i);
  }

  for(unsigned i = 0; i < nLinhas; i++) {
    fprintf(stdout, "Escolhido: %d, Verdadeiro: %s", gID[i], grupoVerdadeiro[i]);
  }

  /* INÍCIO DESALOCAÇÃO DE MEMÓRIA */
  free(qtdExemplosGrupo);
  free(acumulador);
  free(atributo);
  free(gerados);
  free(gID);
  free(aux);

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
