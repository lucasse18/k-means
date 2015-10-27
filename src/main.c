#include "utils.h"
#define MY_BUFSIZ 512

int main(int argc, char* argv[]) {

  if(argc < 4) {
    fprintf(stderr, "Argumentos insuficientes\n");
    exit(EXIT_FAILURE);
  }

  argv++;//descarta o nome do programa

  /*
   * argv[0] = separador
   * argv[1] = diretorio
   * argv[2] = numero de grupos (K)
   * argv[3] = presenca de atributo de classe (=y sim ou !=y nao)
   */

  char *nomeDoArquivo = malloc(MY_BUFSIZ*sizeof(char));

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

  Data data;
  data.nExemplos = 0;
  fscanf(stdin,"%d %d", &data.nExemplos, &data.nAtributos);
  fgetc(stdin);//descarta quebra de linha
  data.K = (unsigned) atoi(argv[2]);

  /* INICIO ALOCACAO MEMORIA */
  long *melhorGrupo = calloc(data.nExemplos , sizeof(*melhorGrupo));

  if(melhorGrupo == NULL) {
    fprintf(stderr, "Erro de alocacao.\n");
    exit(EXIT_FAILURE);
  }

  long *segMelhorGrupo = calloc(data.nExemplos , sizeof(*segMelhorGrupo));
  if(segMelhorGrupo == NULL) {
    fprintf(stderr, "Erro de alocacao.\n");
    exit(EXIT_FAILURE);
  }

  char *comentario = malloc(BUFSIZ*sizeof(char));
  if(comentario == NULL) {
    fprintf(stderr, "Erro de alocacao.\n");
    exit(EXIT_FAILURE);
  }

  float **centros = malloc(data.K*sizeof(*centros));
  if(centros == NULL) {
    fprintf(stderr, "Erro de alocacao.\n");
    exit(EXIT_FAILURE);
  }
  for(int i = 0; i < data.K; i++) {
    centros[i] = malloc(data.nAtributos *sizeof(**centros));
    if(centros[i] == NULL) {
      fprintf(stderr, "Erro de alocacao.\n");
      exit(EXIT_FAILURE);
    }
  }

  float **centrosAnt = malloc(data.K*sizeof(*centrosAnt));
  if(centrosAnt == NULL) {
    fprintf(stderr, "Erro de alocacao.\n");
    exit(EXIT_FAILURE);
  }
  for(int i = 0; i < data.K; i++) {
    centrosAnt[i] = malloc(data.nAtributos *sizeof(**centrosAnt));
    if(centrosAnt[i] == NULL) {
      fprintf(stderr, "Erro de alocacao.\n");
      exit(EXIT_FAILURE);
    }
  }

  float **exemplos = malloc(data.nExemplos *sizeof(*exemplos));
  if(exemplos == NULL) {
    fprintf(stderr, "Erro de alocacao.\n");
    exit(EXIT_FAILURE);
  }
  for(int i = 0; i < data.nExemplos; i++) {
    exemplos[i] = malloc(data.nAtributos *sizeof(**exemplos));
    if(exemplos[i] == NULL) {
      fprintf(stderr, "Erro de alocacao.\n");
      exit(EXIT_FAILURE);
    }
  }

  float *upperBound = malloc(data.nExemplos *sizeof(*upperBound));
  if(upperBound == NULL) {
    fprintf(stderr, "Erro de alocacao.\n");
    exit(EXIT_FAILURE);
  }

  float *lowerBound = malloc(data.nExemplos *sizeof(*lowerBound));
  if(lowerBound == NULL) {
    fprintf(stderr, "Erro de alocacao.\n");
    exit(EXIT_FAILURE);
  }

  float *variacao = malloc(data.K*sizeof(*variacao));
  if(variacao == NULL) {
    fprintf(stderr, "Erro de alocacao.\n");
    exit(EXIT_FAILURE);
  }

  char **grupoVerdadeiro = malloc(data.nExemplos *sizeof(char *));
  if(grupoVerdadeiro == NULL) {
    fprintf(stderr, "Erro de alocacao.\n");
    exit(EXIT_FAILURE);
  }
  for(int i = 0; i < data.nExemplos; i++) {
    grupoVerdadeiro[i] = malloc(MY_BUFSIZ*sizeof(char));
    if(grupoVerdadeiro[i] == NULL) {
      fprintf(stderr, "Erro de alocacao.\n");
      exit(EXIT_FAILURE);
    }
  }

  /* LEITURA DADOS */
  unsigned tamSeparador = (unsigned)strlen(argv[0]);
  for(int i = 0; i < data.nExemplos; i++) {
    if(olhaChar() != '%') {
      for(int j = 0; j < data.nAtributos; j++) {
        fscanf(stdin, "%f", &exemplos[i][j]);
        //fprintf(stdout, "%.2f%s", exemplos[i][j], argv[0]);//teste
        for(int k = 0; k < tamSeparador; k++)
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
  free(comentario);

  //TODO repetir K-MEANS n vezes e extrair melhor RSS
  /* K-MEANS */
  //gera os índices dos elementos usados na criação dos grupos iniciais
  unsigned Seed = (unsigned) seed();
  fprintf(centrosIniciais, "%u\n", Seed);
  srand48(Seed);

  //inicializa centros aleatorios
  InicializaCentros(&centros, exemplos, data, centrosIniciais, argv[0]);
  fclose(centrosIniciais);

  int melhor, trocas = 0;
  long double RSS;
  float menorDistancia, dAtual;
  float *acumulador = calloc(data.K, sizeof(*acumulador));//zerado
  int *qtdExemplosGrupo = calloc(data.K, sizeof(*qtdExemplosGrupo));//zerado

  //TODO usar funcao maisProximo, modularizar
  //atribui
  for(int i = 0; i < data.nExemplos; i++) {
    menorDistancia = distancia(exemplos[i], centros[0], data.nAtributos);
    melhor = 0;
    for(int j = 1; j < data.K; j++) {
      dAtual = distancia(exemplos[i], centros[j], data.nAtributos);
      if(dAtual < menorDistancia) {
        menorDistancia = dAtual;
        melhor = j;
      } else if ( dAtual == menorDistancia) {
        fprintf(stderr,"Warning: distancias iguais: %.4f\n", menorDistancia);
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
    for(int i = 0; i < data.nAtributos; i++) {
      //zera acumuladores antes da proxima iteração
      for(int j = 0; j < data.K; j++)
        acumulador[j] = 0;
      //soma o valor da coordenada i dos exemplos em seu respectivo acumulador
      for(int j = 0; j < data.nExemplos; j++)
        acumulador[melhorGrupo[j]] += exemplos[j][i];
      //atualiza o centro com a media dos pontos
      for(int j = 0; j < data.K && qtdExemplosGrupo[j] > 0; j++) {
        centrosAnt[j][i] = centros[j][i];//salva centro anterior
        centros[j][i] = ((acumulador[j])/qtdExemplosGrupo[j]);
      }
    }

    for(int i = 0; i < data.nExemplos; i++) {
      upperBound[i] = distancia(exemplos[i], centros[melhorGrupo[i]], data.nAtributos);
      lowerBound[i] = distancia(exemplos[i], centros[segMelhorGrupo[i]], data.nAtributos);
    }

    //calcula variacao dos centros
    for(int i = 0; i < data.K; i++) {
      variacao[i] = distancia(centros[i], centrosAnt[i], data.K);
      fprintf(stderr, "V%d:%.4f, ", i, variacao[i]);
    }
    fprintf(stderr,"\n");

    trocas = 0;
    //TODO decrementar qtdExemplosGrupo quando um centro mudar de grupo ao inves de zerar
    for(int i = 0; i < data.K; i++)
      qtdExemplosGrupo[i] = 0;

    //TODO usar funcao maisProximo, modularizar
    //atribui
    for(int i = 0; i < data.nExemplos; i++) {
      if(lowerBound[i] - max(variacao, data.K) >= (upperBound[i] + distancia(
          centros[melhorGrupo[i]], centrosAnt[melhorGrupo[i]], data.nAtributos))) {
        menorDistancia = distancia(exemplos[i], centros[0], data.nAtributos);
        melhor = 0;
        for(int j = 1; j < data.K; j++) {
          dAtual = distancia(exemplos[i], centros[j], data.nAtributos);
          if(dAtual < menorDistancia) {
            menorDistancia = dAtual;
            melhor = j;
          }
          else if (dAtual == menorDistancia) {
            fprintf(stderr, "Warning2: distancias iguais: %.4f\n", menorDistancia);
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
    RSS = 0;
    for(int i = 0; i < data.nExemplos; i++)
      RSS += upperBound[i];

    fprintf(stderr, "RSS: %Lf\n", RSS);
  }
  //TODO repetir K-MEANS n vezes e extrair melhor RSS

  /* SALVA RESULTADOS */
  //salva centros finais
  for(int i = 0; i < data.K; i++) {
    for(int j = 0; j < data.nAtributos; j++) {
      fprintf(centrosFile, "%.4f", centros[i][j]);
      if(j != data.nAtributos - 1)
        fprintf(centrosFile, "%s", argv[0]);
    }
    fputc('\n', centrosFile);
  }

  //salva atribuicoes
  for(int i = 0; i < data.nExemplos; i++)
    fprintf(atribuicoesFile, "%ld\n", melhorGrupo[i]);

  fclose(centrosFile);
  fclose(atribuicoesFile);

  for(int i = 0; i < data.K; i++)
    fprintf(stderr, "%d exemplos no grupo %d\n", qtdExemplosGrupo[i], i);

  if(argv[3][0] == 'y') {
    for(int i = 0; i < data.nExemplos; i++)
      fprintf(stdout, "Escolhido: %ld, Verdadeiro: %s", melhorGrupo[i], grupoVerdadeiro[i]);
  }

  free(qtdExemplosGrupo);
  free(acumulador);
  free(melhorGrupo);
  free(segMelhorGrupo);
  free(upperBound);
  free(lowerBound);
  free(variacao);

  for(int i = 0; i < data.K; i++)
    free(centrosAnt[i]);
  free(centrosAnt);

  for(int i = 0; i < data.K; i++)
    free(centros[i]);
  free(centros);

  for(int i = 0; i < data.nExemplos; i++)
    free(exemplos[i]);
  free(exemplos);

  for(int i = 0; i < data.nExemplos; i++)
    free(grupoVerdadeiro[i]);
  free(grupoVerdadeiro);

  return 0;
}
