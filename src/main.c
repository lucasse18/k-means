#include "utils.h"
#define MY_BUFSIZ 512

int main(int argc, char* argv[]) {

  if(argc < 5) {
    fprintf(stderr, "Argumentos insuficientes\n");
    exit(EXIT_FAILURE);
  }

  argv++;//descarta o nome do programa

  /*
   * argv[0] = separador
   * argv[1] = diretorio
   * argv[2] = dataset
   * argv[3] = numero de grupos (K)
   * argv[4] = presenca de atributo de classe (=y sim ou !=y nao)
   */

  /* INICIO ABERTURA ARQUIVOS */
  char *nomeDoArquivo = malloc(MY_BUFSIZ * sizeof(char));

  sprintf(nomeDoArquivo, "%s/%s", argv[1], argv[2]);
  FILE *entrada = fopen(nomeDoArquivo, "r");
  if(entrada == NULL) {
    fprintf(stderr, "Erro ao abrir arquivo %s", nomeDoArquivo);
    exit(EXIT_FAILURE);
  }

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
  free(nomeDoArquivo);
  /* FIM ABERTURA ARQUIVOS */

  /* INICIO INICIALIZACAO DATASET */
  Data data;
  data.nExemplos = 0;
  fscanf(entrada,"%d %d", &data.nExemplos, &data.nAtributos);
  fgetc(entrada);//descarta quebra de linha
  data.K = (unsigned) atoi(argv[3]);
  /* FIM INICIALIZACAO DATASET */

  /* INICIO ALOCACAO MEMORIA */
  long *melhorGrupo = calloc(data.nExemplos, sizeof(*melhorGrupo));

  if(melhorGrupo == NULL) {
    fprintf(stderr, "Erro de alocacao.\n");
    exit(EXIT_FAILURE);
  }

  long *segMelhorGrupo = calloc(data.nExemplos, sizeof(*segMelhorGrupo));
  if(segMelhorGrupo == NULL) {
    fprintf(stderr, "Erro de alocacao.\n");
    exit(EXIT_FAILURE);
  }

  char *comentario = malloc(BUFSIZ * sizeof(char));
  if(comentario == NULL) {
    fprintf(stderr, "Erro de alocacao.\n");
    exit(EXIT_FAILURE);
  }

  double **centros = calloc(data.K, sizeof(*centros));
  if(centros == NULL) {
    fprintf(stderr, "Erro de alocacao.\n");
    exit(EXIT_FAILURE);
  }
  for(unsigned i = 0; i < data.K; i++) {
    centros[i] = calloc(data.nAtributos, sizeof(**centros));
    if(centros[i] == NULL) {
      fprintf(stderr, "Erro de alocacao.\n");
      exit(EXIT_FAILURE);
    }
  }

  double **centrosAnt = calloc(data.K, sizeof(*centrosAnt));
  if(centrosAnt == NULL) {
    fprintf(stderr, "Erro de alocacao.\n");
    exit(EXIT_FAILURE);
  }
  for(unsigned i = 0; i < data.K; i++) {
    centrosAnt[i] = calloc(data.nAtributos, sizeof(**centrosAnt));
    if(centrosAnt[i] == NULL) {
      fprintf(stderr, "Erro de alocacao.\n");
      exit(EXIT_FAILURE);
    }
  }

  double **exemplos = calloc(data.nExemplos, sizeof(*exemplos));
  if(exemplos == NULL) {
    fprintf(stderr, "Erro de alocacao.\n");
    exit(EXIT_FAILURE);
  }
  for(unsigned i = 0; i < data.nExemplos; i++) {
    exemplos[i] = calloc(data.nAtributos, sizeof(**exemplos));
    if(exemplos[i] == NULL) {
      fprintf(stderr, "Erro de alocacao.\n");
      exit(EXIT_FAILURE);
    }
  }

  double *upperBound = malloc(data.nExemplos * sizeof(*upperBound));
  if(upperBound == NULL) {
    fprintf(stderr, "Erro de alocacao.\n");
    exit(EXIT_FAILURE);
  }

  double *lowerBound = malloc(data.nExemplos * sizeof(*lowerBound));
  if(lowerBound == NULL) {
    fprintf(stderr, "Erro de alocacao.\n");
    exit(EXIT_FAILURE);
  }

  double *variacao = calloc(data.K, sizeof(*variacao));
  if(variacao == NULL) {
    fprintf(stderr, "Erro de alocacao.\n");
    exit(EXIT_FAILURE);
  }

  char **grupoVerdadeiro = malloc(data.nExemplos * sizeof(char *));
  if(grupoVerdadeiro == NULL) {
    fprintf(stderr, "Erro de alocacao.\n");
    exit(EXIT_FAILURE);
  }
  for(unsigned i = 0; i < data.nExemplos; i++) {
    grupoVerdadeiro[i] = malloc(MY_BUFSIZ * sizeof(char));
    if(grupoVerdadeiro[i] == NULL) {
      fprintf(stderr, "Erro de alocacao.\n");
      exit(EXIT_FAILURE);
    }
  }

  double *acumulador = calloc(data.K, sizeof(*acumulador));
  if(acumulador == NULL) {
    fprintf(stderr, "Erro de alocacao.\n");
    exit(EXIT_FAILURE);
  }

  int *qtdExemplosGrupo = calloc(data.K, sizeof(*qtdExemplosGrupo));
  if(qtdExemplosGrupo == NULL) {
    fprintf(stderr, "Erro de alocacao.\n");
    exit(EXIT_FAILURE);
  }
  /* FIM ALOCACAO MEMORIA */

  /* INICIO LEITURA DADOS */
  unsigned tamSeparador = (unsigned)strlen(argv[0]);
  for(unsigned i = 0; i < data.nExemplos; i++) {
    if(olhaChar(entrada) != '%') {
      for(unsigned j = 0; j < data.nAtributos; j++) {
        fscanf(entrada, "%lf", &exemplos[i][j]);
        for(unsigned k = 0; k < tamSeparador; k++)
          fgetc(entrada);//retira separador
      }

      //retira e guarda coluna de classe se argv[3][0] for igual a y
      if(argv[4][0] == 'y')
        fgets(grupoVerdadeiro[i], MY_BUFSIZ, entrada);
    }
    else {
      fgets(comentario, MY_BUFSIZ, entrada);
      i--;
    }
  }
  free(comentario);
  fclose(entrada);
  comentario = NULL;
  /* FIM LEITURA DADOS */

  //TODO repetir K-MEANS n vezes e extrair melhor RSS
  /* K-MEANS */

  clock_t inicio, fim;
  inicio = clock();
  if(inicio == -1) {
    perror("inicio clock\n");
    exit(EXIT_FAILURE);
  }

  unsigned Seed = (unsigned) seed();
  fprintf(centrosIniciais, "Seed: %u\n", Seed);
  srand48(Seed);

  //inicializa centros
  InicializaCentrosPP(centros, exemplos, data, centrosIniciais, argv[0]);
  fclose(centrosIniciais);

  long double RSS = 0.0;
  int melhor, trocas = 0;
  double menorDistancia, dAtual;


  //classico
  //----------------------------------------------------------------------------
  //atribui
  /* for(unsigned i = 0; i < data.nExemplos; i++) { */
  /*   melhor = 0; */
  /*   menorDistancia = distancia(exemplos[i], centros[0], data.nAtributos); */
  /*   for(unsigned j = 1; j < data.K; j++) { */
  /*     dAtual = distancia(exemplos[i], centros[j], data.nAtributos); */
  /*     if(dAtual < menorDistancia) { */
  /*       menorDistancia = dAtual; */
  /*       melhor = j; */
  /*     } */
  /*   } */
  /*   if(melhorGrupo[i] != melhor) { */
  /*     trocas = 1; */
  /*     melhorGrupo[i] = melhor; */
  /*   } */
  /*   qtdExemplosGrupo[melhor]++; */
  /* } */

  /* while(trocas) { */
  /*   //recomputa */
  /*   for(unsigned i = 0; i < data.nAtributos; i++) { */
  /*     //zera acumuladores antes da proxima iteração */
  /*     for(unsigned j = 0; j < data.K; j++) */
  /*       acumulador[j] = 0; */
  /*     //soma o valor da coordenada i dos exemplos em seu respectivo acumulador */
  /*     for(unsigned j = 0; j < data.nExemplos; j++) */
  /*       acumulador[melhorGrupo[j]] += exemplos[j][i]; */
  /*     //atualiza o centro com a media dos pontos */
  /*     for(unsigned j = 0; j < data.K; j++) { */
  /*       if(qtdExemplosGrupo[j] > 0) */
  /*         centros[j][i] = ((acumulador[j])/qtdExemplosGrupo[j]); */
  /*     } */
  /*   } */

  /*   //zera contadores */
  /*   RSS = 0; */
  /*   trocas = 0; */
  /*   for(unsigned i = 0; i < data.K; i++) */
  /*     qtdExemplosGrupo[i] = 0; */

  /*   //atribui */
  /*   for(unsigned i = 0; i < data.nExemplos; i++) { */
  /*     menorDistancia = distancia(exemplos[i], centros[0], data.nAtributos); */
  /*     melhor = 0; */
  /*     for(unsigned j = 1; j < data.K; j++) { */
  /*       dAtual = distancia(exemplos[i], centros[j], data.nAtributos); */
  /*       if(dAtual < menorDistancia) { */
  /*         menorDistancia = dAtual; */
  /*         melhor = j; */
  /*       } */
  /*     } */
  /*     if(melhorGrupo[i] != melhor) { */
  /*       melhorGrupo[i] = melhor; */
  /*       trocas = 1; */
  /*     } */
  /*     qtdExemplosGrupo[melhor]++; */
  /*     RSS += menorDistancia; */
  /*   } */

  /*   fprintf(stderr, "RSS: %Lf\n", RSS); */
  /* } */
  //----------------------------------------------------------------------------


  //YY
  //----------------------------------------------------------------------------
  //atribui
  for(unsigned i = 0; i < data.nExemplos; i++) {
    menorDistancia = distancia(exemplos[i], centros[0], data.nAtributos);
    melhor = 0;
    for(unsigned j = 1; j < data.K; j++) {
      dAtual = distancia(exemplos[i], centros[j], data.nAtributos);
      if(dAtual < menorDistancia) {
        menorDistancia = dAtual;
        melhor = j;
      }
      else if ( dAtual == menorDistancia) {
        fprintf(stderr,"Warning: distancias iguais: %.4f\n", menorDistancia);
      }
    }
    if(melhorGrupo[i] != melhor) {
      trocas = 1;
      segMelhorGrupo[i] = melhorGrupo[i];
      melhorGrupo[i] = melhor;
    }
    qtdExemplosGrupo[melhor]++;
  }

  while(1) {
    //verifica se houve mudança
    if(!trocas)
      break;//FIXME nao calcula RSS se nao iterar 1 vez

    //recomputa
    for(unsigned i = 0; i < data.nAtributos; i++) {
      //zera acumuladores antes da proxima iteração
      for(unsigned j = 0; j < data.K; j++)
        acumulador[j] = 0;
      //soma o valor da coordenada i dos exemplos em seu respectivo acumulador
      for(unsigned j = 0; j < data.nExemplos; j++)
        acumulador[melhorGrupo[j]] += exemplos[j][i];
      //atualiza o centro com a media dos pontos
      for(unsigned j = 0; j < data.K && qtdExemplosGrupo[j] > 0; j++) {
        centrosAnt[j][i] = centros[j][i];//salva centro anterior
        centros[j][i] = ((acumulador[j])/qtdExemplosGrupo[j]);
      }
    }

    for(unsigned i = 0; i < data.nExemplos; i++) {
      upperBound[i] = distancia(exemplos[i], centros[melhorGrupo[i]], data.nAtributos);
      lowerBound[i] = distancia(exemplos[i], centros[segMelhorGrupo[i]], data.nAtributos);
    }

    //calcula variacao dos centros
    for(unsigned i = 0; i < data.K; i++)
      variacao[i] = distancia(centros[i], centrosAnt[i], data.K);

    trocas = 0;
    //TODO decrementar qtdExemplosGrupo quando um centro mudar de grupo ao inves de zerar
    for(unsigned i = 0; i < data.K; i++)
      qtdExemplosGrupo[i] = 0;

    //TODO usar funcao maisProximo, modularizar
    //atribui
    RSS = 0.0;
    for(unsigned i = 0; i < data.nExemplos; i++) {
      if(lowerBound[i] - max(variacao, data.K) >= (upperBound[i] + distancia(
          centros[melhorGrupo[i]], centrosAnt[melhorGrupo[i]], data.nAtributos))) {
        melhor = 0;
        menorDistancia = distancia(exemplos[i], centros[0], data.nAtributos);
        for(unsigned j = 1; j < data.K; j++) {
          dAtual = distancia(exemplos[i], centros[j], data.nAtributos);
          if(dAtual < menorDistancia) {
            menorDistancia = dAtual;
            melhor = j;
          }
          /* else if (dAtual == menorDistancia) { */
          /*   fprintf(stderr, "Warning2: distancias iguais: %.4f\n", menorDistancia); */
          /* } */
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
      RSS += upperBound[i];
    }
    fprintf(stderr, "RSS: %Lf\n", RSS);
  }
  //----------------------------------------------------------------------------

  fim = clock();
  if(fim == -1) {
    perror("fim clock");
    fprintf(stderr, "Nao foi possivel calcular o tempo de execucao.\n");
  }
  else
    fprintf(stderr, "Tempo de execucao: %.15f\n", (double) (fim-inicio)/CLOCKS_PER_SEC);

  /* FIM K-MEANS */
  //TODO repetir K-MEANS n vezes e extrair melhor RSS

  /* INICIO SALVA RESULTADOS */
  for(unsigned i = 0; i < data.K; i++) {
    for(unsigned j = 0; j < data.nAtributos; j++) {
      fprintf(centrosFile, "%.4f", centros[i][j]);
      if(j != data.nAtributos - 1)
        fprintf(centrosFile, "%s", argv[0]);
    }
    fputc('\n', centrosFile);
  }

  for(unsigned i = 0; i < data.nExemplos; i++)
    fprintf(atribuicoesFile, "%ld\n", melhorGrupo[i]);

  fclose(centrosFile);
  fclose(atribuicoesFile);

  for(unsigned i = 0; i < data.K; i++)
    fprintf(stderr, "%d exemplos no grupo %u\n", qtdExemplosGrupo[i], i);

  if(argv[3][0] == 'y') {
    for(unsigned i = 0; i < data.nExemplos; i++)
      fprintf(stdout, "Escolhido: %ld, Verdadeiro: %s", melhorGrupo[i], grupoVerdadeiro[i]);
  }
  else {
    for(unsigned i = 0; i < data.nExemplos; i++)
      fprintf(stdout, "Escolhido: %ld", melhorGrupo[i]);
  }
  /* FIM SALVA RESULTADOS */

  /* INICIO DESALOCA MEMORIA */
  free(qtdExemplosGrupo);
  free(acumulador);
  free(melhorGrupo);
  free(segMelhorGrupo);
  free(upperBound);
  free(lowerBound);
  free(variacao);

  for(unsigned i = 0; i < data.K; i++)
    free(centrosAnt[i]);
  free(centrosAnt);

  for(unsigned i = 0; i < data.K; i++)
    free(centros[i]);
  free(centros);

  for(unsigned i = 0; i < data.nExemplos; i++)
    free(exemplos[i]);
  free(exemplos);

  for(unsigned i = 0; i < data.nExemplos; i++)
    free(grupoVerdadeiro[i]);
  free(grupoVerdadeiro);
  /* FIM DESALOCA MEMORIA */

  return 0;
}
