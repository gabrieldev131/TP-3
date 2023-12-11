#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "exames.h"

#define MAX_QUANTIDADE_UNIDADE_DE_TEMPO 43200
#define MAX_QUANTIDADE_UNIDADE_DE_TEMPO_ESPERA 7200
#define MAX_QUANTIDADE_RAIOX 5
#define MAX_QUANTIDADE_LAUDO 3
#define MAX_QUANTIDADE_PATOLOGIAS 5
#define MAX_QUANTIDADE_EVENTOS 9504

struct node {
  void *dados;
  Node *proximo;
  Node *anterior;
};

struct lista {
  Node *Primeiro;
  Node *Ultimo;
  int tamanho;
};

struct fila {
  Node *Primeiro;
  Node *Ultimo;
  int tamanho;
};

struct hospital{
  int raioX[MAX_QUANTIDADE_RAIOX][3]; //col1 = se está disponivel, col2 = tempo que será usada, col3 = id do paciente
  int laudo[MAX_QUANTIDADE_LAUDO][3]; //col1 = se está disponivel, col2 = tempo que será usada, col3 = id do paciente
  float **registro_pacientes;//col1 = patologia, col2 = tempo
};

struct paciente {
  char nome[50];
  char cpf[15];
  int idade;
  int id;
  int patologia[2]; //id da patologia e a gravidade
  int entrada, entrada_raiox, saida_raiox, entrada_laudo, saida;
};

struct log_Evento{
char message[256];
};

struct log{
Log_Evento eventos[MAX_QUANTIDADE_EVENTOS]; 
/*
10% a mais do que a média de pacientes (Distribuição binomial)
média de 8640 pacientes esperados, 
desvio padrão de aproximadamente 12.65 U.T
*/
int count;
};

/*-----------------------------------------------------------------
                            Lista
-----------------------------------------------------------------*/

Lista *inicia_Lista(){
    Lista *lista = (Lista *)malloc(sizeof(Lista));
    lista->Primeiro = NULL;
    lista->Ultimo = NULL;
    lista->tamanho = 0;
    return lista;
}

void insere_Lista(Lista *lista, void *dados) {
    Node *newNode = (Node*)malloc(sizeof(Node));
    newNode->dados = dados;
    newNode->proximo = lista->Primeiro;
    newNode->anterior = NULL;
    if(lista->tamanho != 0) {
      lista->Primeiro->anterior = newNode;
    }
    else{
      lista->Ultimo = newNode;
    }
    lista->Primeiro = newNode;
    lista->tamanho++;
}

void remove_dados_lista(Lista *l, void *val) {
  Node *temp = l->Primeiro;
  Node *anterior = NULL;
  Node *auxiliar;
  while (temp != NULL) {
    if (temp->dados == val) {
      auxiliar = temp;
      if (anterior != NULL) {
        anterior->proximo = temp->proximo;
        if (temp->proximo == NULL) {
          l->Ultimo = anterior;
        }
      } else {
        l->Primeiro = temp->proximo;
        if (temp->proximo == NULL) {
          l->Ultimo = NULL;
        }
      }
      temp = temp->proximo;
      free(auxiliar);
    } 
    else {
      anterior = temp;
      temp = temp->proximo;
    }
  }
  l->tamanho--;
}

void liberaLista(Lista *lista) {
    Node *primeiro = lista->Primeiro;
    Node *proximo;
    while (primeiro != NULL) {
        proximo = primeiro->proximo;
        free(primeiro);
        primeiro = proximo;
    }
    free(lista);
}

/*----------------------------------------------------------------
                            Fila
-----------------------------------------------------------------*/

Fila *inicia_Fila(){
  Fila *fila = (Fila *)malloc(sizeof(Fila));
    fila->Ultimo = NULL;
    fila->Primeiro = NULL;
    fila->tamanho = 0;
    return fila;
}

void remove_dados_fila(Fila *fila) {
    if (fila->Primeiro == NULL) {
        fprintf(stderr, "Erro: Fila vazia\n");
        return;
    }
    Node *temp = fila->Primeiro;
    fila->Primeiro = fila->Primeiro->proximo;
    // Se a fila ficar vazia após a remoção
    if (fila->Primeiro == NULL) {
        fila->Ultimo = NULL;
    } 
    else {
        fila->Primeiro->anterior = NULL;
    }
    free(temp);              // Liberar o nó removido
    fila->tamanho--;
}

void liberaFila(Fila *fila) {
    Node *primeiro = fila->Primeiro;
    Node *proximo;
    while (primeiro != NULL) {
        proximo = primeiro->proximo;
        free(primeiro);
        primeiro = proximo;
    }
    free(fila);
}

/*---------------------------------------------------------------
                              TAD Hospital
---------------------------------------------------------------*/

Hospital *inicia_Hospital() {
  Hospital *hospital = (Hospital *)malloc(sizeof(Hospital));
  //confere se conseguiu alocar
  if (hospital == NULL) {
    printf("Erro: falha ao alocar memória para Hospital.\n");
    exit(EXIT_FAILURE);
  }
  hospital->registro_pacientes = (float**)malloc(MAX_QUANTIDADE_PATOLOGIAS * sizeof(int*));
  //conferindo se alocou os ponteiros de ponteiros
  if (hospital->registro_pacientes == NULL) {
      printf("Erro: falha ao alocar memória.\n");
      exit(EXIT_FAILURE);
  }
  for (int i = 0; i < MAX_QUANTIDADE_PATOLOGIAS; i++) {
    hospital->registro_pacientes[i] = (float *)malloc(sizeof(float) * 2);
    //conferindo se alocou cada elemento da matriz
    if (hospital->registro_pacientes[i] == NULL) {
      printf("Erro: falha ao alocar memória para registro_pacientes[%d].\n", i);
      // Liberar memória anterioriamente alocada
      liberaHospital(hospital);
      exit(EXIT_FAILURE);
    }
    hospital->registro_pacientes[i][0] = i+1;
    hospital->registro_pacientes[i][1] = 0;
  }
  return hospital;
}

void liberaHospital(Hospital *hospital) {
  if (hospital != NULL) {
    for (int i = 0; i < MAX_QUANTIDADE_PATOLOGIAS; i++) {
      free(hospital->registro_pacientes[i]);
    }
    free(hospital->registro_pacientes);
    free(hospital);
  }
}

int probabilidade_paciente(){
  int probabilidade_paciente = rand() % 100;
  if(probabilidade_paciente < 20){
    return 1;
  }
  return 0;
}

int *probabilidade_patologia(){
  int probabilidade_patologia = rand() % 100;
  int *patologia = (int*)malloc(sizeof(int)*2); //váriavel que será retornada tem 2 valores
  int SAUDE_NORMAL = 1;
  int BRONQUITE = 2;
  int PNEUMONIA = 3;
  int FRATURA_DE_FEMUR = 4;
  int APENDICITE = 4;

  if(probabilidade_patologia < 30 ){
    patologia[0] = 1;
    patologia[1] = SAUDE_NORMAL;
  }
  else if (probabilidade_patologia >= 30 && probabilidade_patologia < 50){
    patologia[0] = 2;
    patologia[1] = BRONQUITE;
  }
  else if (probabilidade_patologia >= 50 && probabilidade_patologia < 70){
    patologia[0] = 3;
    patologia[1] = PNEUMONIA;
  }
  else if (probabilidade_patologia >= 70 && probabilidade_patologia < 85){
    patologia[0] = 4;
    patologia[1] = FRATURA_DE_FEMUR;
  }
  else if (probabilidade_patologia >= 85){
    patologia[0] = 5;
    patologia[1] = APENDICITE;
  }
  return patologia;
}

int probabilidade_raiox(){
  int probabilidade_raiox = 5 + (rand() % 6);
  return probabilidade_raiox;
}

int probabilidade_laudo(){
  int probabilidade_laudo = 10 + rand() % 21;
  return probabilidade_laudo;
}

char* gerarNome() {
    char *nomes[] = {"joao", "Maria", "Carlos", "Ana", "Pedro", "julia", "Lucas", "Mariana", "Guilherme", "Emilly", "Gabriel"};
    // Gera um índice aleatório para escolher um nome da lista
    int indice = rand() % (sizeof(nomes) / sizeof(nomes[0]));
    // copia e cola o nome na váriavel nome
    char* nome = (char*)malloc(sizeof(char) * 10);
    nome = strdup(nomes[indice]);
    return nome;
}

char* gerarCPF() {
    char cpf[20];//usei 20 porque o compilador dav aviso, mas rodava
    // Gera números aleatórios para o CPF
    sprintf(cpf, "%03d.%03d.%03d-%02d", rand() % 1000, rand() % 1000, rand() % 1000, rand() % 100);//pedia 19 espaços de memória no minimo
    // copia e cola o cpf na váriavel resultado
    char* resultado = (char*)malloc(sizeof(char)*15);
    resultado = strdup(cpf);
    return resultado;
}

int gerarIdade() {
  return rand() % 100;
}

Paciente *gerarPaciente(int ID){
  Paciente *paciente = (Paciente *)malloc(sizeof(Paciente));
  if (paciente == NULL) {
    fprintf(stderr, "Erro ao alocar memória para Paciente\n");
    exit(EXIT_FAILURE);
  }
  int *patologia = (int*)malloc(sizeof(int)*2);
  patologia = probabilidade_patologia();
  // Gera aleatoriamente o nome, CPF e idade
  strcpy(paciente->nome, gerarNome());
  strcpy(paciente->cpf, gerarCPF());
  paciente->idade = gerarIdade();
  paciente->id = ID;
  paciente->patologia[0] = patologia[0];
  paciente->patologia[1] = patologia[1];
  paciente->entrada = ID;
  free(patologia);
  return paciente;
}

void fila_atendimento_raiox(Fila *fila_sala_de_entrada, Fila *raiox, Hospital *hospital, int Unidade_de_tempo){
  //verifica se tem algum paciente na entrada do hospital
  if(fila_sala_de_entrada->Primeiro == NULL){
    return;
  }
  Paciente *auxiliar = (Paciente*)fila_sala_de_entrada->Primeiro->dados;
  for (int i = 0;i<MAX_QUANTIDADE_RAIOX;i++){
    if(hospital->raioX[i][0] == 0){
      hospital->raioX[i][0] = 1;
      hospital->raioX[i][1] = probabilidade_raiox();
      hospital->raioX[i][2] = auxiliar->id;
      auxiliar->entrada_raiox = Unidade_de_tempo;
      remove_dados_fila(fila_sala_de_entrada);
      insere_ordenado(raiox, auxiliar);
      return; //termina o loop e retorna
    }
  }
}

void fila_atendimento_laudo(Fila *fila_raiox, Fila *laudo, Hospital *hospital, int Unidade_de_tempo){
  if (fila_raiox->Primeiro == NULL) {
    return; // Não há pacientes no raio-x, portanto, não é possível ir para o laudo
  }

  Paciente *auxiliar = (Paciente *)fila_raiox->Primeiro->dados;
  if (auxiliar->saida_raiox == 0) {
    return; // O paciente ainda não passou pelo raio-x, então não pode ir para o laudo
  }

  for (int i = 0; i < MAX_QUANTIDADE_LAUDO; i++) {
    if (hospital->laudo[i][0] == 0) {
      hospital->laudo[i][0] = 1;
      hospital->laudo[i][1] = probabilidade_laudo();
      hospital->laudo[i][2] = auxiliar->id;
      auxiliar->entrada_laudo = Unidade_de_tempo;
      remove_dados_fila(fila_raiox);
      insere_ordenado(laudo, auxiliar);
      return; //termina o loop e retorna
    }
  }
}

void atualiza_tempo_raiox(Hospital *hospital){
  for(int i = 0; i<MAX_QUANTIDADE_RAIOX; i++){
    if(hospital->raioX[i][1] != 0){
      hospital->raioX[i][1]--;
    }
    if(hospital->raioX[i][1] == 0){
      hospital->raioX[i][0] = 0;
    }
  }
}

void atualiza_tempo_laudo(Hospital *hospital){
  for(int i = 0;i<MAX_QUANTIDADE_LAUDO;i++){
    if(hospital->laudo[i][1] != 0){
      hospital->laudo[i][1]--;
    }
    if(hospital->laudo[i][1] == 0){
      hospital->laudo[i][0] = 0;
    }
  }
}

void atualiza_raiox(Hospital *hospital, Fila *fila_raiox, int Unidade_de_tempo){
  for(int i = 0;i<MAX_QUANTIDADE_RAIOX;i++){
    if(hospital->raioX[i][1] == 0){
      Node *Auxiliar = fila_raiox->Primeiro;
      Paciente *paciente;
      while (Auxiliar != NULL) {
        paciente = (Paciente *)Auxiliar->dados;
        if (paciente->id == hospital->raioX[i][2]) {
          paciente->saida_raiox = Unidade_de_tempo;
          hospital->raioX[i][2] = 0;
        }
        Auxiliar = Auxiliar->proximo;
      }
    }
  }
}

void atualiza_laudo(Hospital *hospital, Fila *fila_laudo, int Unidade_de_tempo){
  for(int i = 0;i<MAX_QUANTIDADE_LAUDO;i++){
    if(hospital->laudo[i][1] == 0){
      Node *Auxiliar = fila_laudo->Primeiro;
      while (Auxiliar != NULL) {
        Paciente *paciente = (Paciente *)Auxiliar->dados;
        if (paciente->id == hospital->laudo[i][2]) {
          paciente->saida = Unidade_de_tempo;
          hospital->laudo[i][2] = 0;
          
          return;
        }
        Auxiliar = Auxiliar->proximo;
      }
    }
  }
}

int verifica_raiox(Hospital *hospital){
  for (int i = 0; i < MAX_QUANTIDADE_RAIOX; i++) {
    if (hospital->raioX[i][0] == 0) {
      return 1;  // Encontrou uma vaga no raio-x
    }
  }
  return 0; // Não encontrou vaga no raio-x
}

int verifica_laudo(Hospital *hospital){
  for (int i = 0; i < MAX_QUANTIDADE_LAUDO; i++) {
    if (hospital->laudo[i][0] == 0) {
      return 1;  // Encontrou uma vaga no laudo
    }
  }
  return 0; // Não encontrou vaga no laudo
}

void atualiza_lista_controle(Fila *fila_laudo, Lista *lista_controle){
  if(fila_laudo->Primeiro == NULL || lista_controle->Primeiro == NULL){
    return;
  }
  Node *auxiliar = fila_laudo->Primeiro;

  if(((Paciente*)auxiliar->dados)->saida == 0) return;

  
  for(Node *Dados_atualizados = lista_controle->Primeiro; Dados_atualizados != NULL; Dados_atualizados = Dados_atualizados->proximo){
    if(((Paciente*)Dados_atualizados->dados)->id == ((Paciente*)auxiliar->dados)->id){
      Dados_atualizados->dados = auxiliar->dados; //troca apenas os dados
      remove_dados_fila(fila_laudo);//tira da fila, o paciente já foi atendido
      return;
    }
  }
}

float media_laudo(Lista *controle){
  float Soma_Tempo = 0.0;
  int Ocorrencias = 0;
  Node *auxiliar = controle->Primeiro;
  Paciente *paciente;
  while(auxiliar!=NULL) {
    paciente = (Paciente*)auxiliar->dados;
    if (paciente->saida != 0 && paciente->saida - paciente->entrada < 7200) {
      Soma_Tempo += paciente->saida - paciente->entrada;
      Ocorrencias++;
    }
    auxiliar = auxiliar->proximo;
  }
  // Evita divisão por zero
  float media;
  if(Ocorrencias!=0){
    media = Soma_Tempo / Ocorrencias;
  } 
  else{
    media = 0.0;
  }
  return media;
} 

void media_patologias(Lista *controle, Hospital *hospital) {
  if (controle->Primeiro == NULL) {
      return;
  }
  Node *auxiliar = controle->Primeiro;
  for (int i = 0; i < MAX_QUANTIDADE_PATOLOGIAS; i++) {
    float soma_tempos = 0.0;
    int contador = 0;
    // Percorre a lista
    while (auxiliar != NULL) {
        Paciente *paciente = (Paciente *)auxiliar->dados;
        // Verifica se o paciente possui a patologia e a qual Unidade_de_tempo foi atendido
        if (paciente->saida != 0 && paciente->patologia[0] == hospital->registro_pacientes[i][0] && 
        paciente->saida - paciente->entrada<=7200) {
          soma_tempos += paciente->saida - paciente->entrada;
          contador++;
        }
        auxiliar = auxiliar->proximo;
    }
    // Calcula a média se houver pacientes
    hospital->registro_pacientes[i][1] = (contador != 0) ? (soma_tempos / contador) : 0.0;
    // Reinicia o ponteiro para o início da lista
    auxiliar = controle->Primeiro;
  }
}

int exames_apos_tempo(Lista *controle){
  int contador = 0;
  Node *auxiliar = controle->Primeiro;
  Paciente *paciente;
  for (int i = 0; i < controle->tamanho; i++) {
    paciente = auxiliar->dados;
    if (paciente->saida != 0 && (paciente->saida - paciente->entrada) >7200) {
      contador++;
    }
    auxiliar = auxiliar->proximo;
  }
  return contador;
} 

void printa_metrica(Lista *controle, Hospital *hospital){
  media_patologias(controle,  hospital);
  printf("número de pacientes: %d\nlaudo: %f\npatologias:\nSaúde normal: %f\nBronquite: %f\nPneumonia: %f\nFratura de fêmur: %f\nApendicite: %f\nFora do tempo: %d\n\n", 
  controle->tamanho, media_laudo(controle),  hospital->registro_pacientes[0][1], 
  hospital->registro_pacientes[1][1], hospital->registro_pacientes[2][1], hospital->registro_pacientes[3][1], 
  hospital->registro_pacientes[4][1], exames_apos_tempo(controle));
}

//coloquei isso tudo aqui para o arquivo main.c ficar mais limpo
void atualiza_hospital(Hospital *hospital, Fila *fila_SE, Fila *fila_raiox,Fila *fila_laudo, int Unidade_de_tempo){
  atualiza_tempo_raiox(hospital);
  atualiza_tempo_laudo(hospital);
  atualiza_raiox(hospital, fila_raiox, Unidade_de_tempo);
  atualiza_laudo(hospital, fila_laudo, Unidade_de_tempo);

  if(verifica_raiox(hospital)){
    //se tiver vagas no raiox, entra no raiox
    fila_atendimento_raiox(fila_SE, fila_raiox, hospital, Unidade_de_tempo);
    }

  if(verifica_laudo(hospital)){
    //se tiver feito raiox e o consultório de consulta estiver vazio, entra no consutório
    fila_atendimento_laudo(fila_raiox, fila_laudo, hospital, Unidade_de_tempo);
  }
}

void gera_paciente(Lista *lista_controle, Fila *fila_SE, int Unidade_de_tempo){
  if(probabilidade_paciente()){  
      Paciente *paciente = gerarPaciente(Unidade_de_tempo);
      insere_Lista(lista_controle, paciente);

      //coloca já ordenado na fila, por ordem de gravidade da doença
      insere_ordenado(fila_SE, paciente);
    }
}
// a função ficou grande porque ela olha de 2 lados
// se for saúde normal ou bronquite, ela começa do final, senão, ela começa do inicio
//isso aumentou a aeficiência
void insere_ordenado(Fila *fila, Paciente *novo_paciente) {
  // Aloca memória para o novo nó
  Node *novo_node = (Node*)malloc(sizeof(Node));
  // Configuração do novo nó
  novo_node->dados = novo_paciente;
  novo_node->anterior = NULL;
  novo_node->proximo = NULL;
  // Caso especial: fila vazia
  if (fila->Primeiro == NULL) {
      fila->Primeiro = novo_node;
      fila->Ultimo = novo_node;
      fila->tamanho++;
      return;
  }
  if(novo_paciente->patologia[0]<=2){
    // Procura a posição correta para inserir o novo paciente
    Node *atual = fila->Ultimo;
    while (atual != NULL && novo_paciente->patologia[1] <= ((Paciente *)atual->dados)->patologia[1]) {
      atual = atual->anterior;
    }
    // Insere o novo_node na posição correta
    if (atual != NULL) {
      Node *anterior = atual->anterior;
      novo_node->proximo = atual;
      novo_node->anterior = anterior;
      if (anterior != NULL) {
          anterior->proximo = novo_node;
      } 
      else {
        // Se anterior é nulo, então o novo_node se torna o novo Primeiro
        fila->Primeiro = novo_node;
      }
      atual->anterior = novo_node;
    } 
    else {
      // Se chegou ao final da fila, insere no final
      Node *ultimo = fila->Ultimo;
      novo_node->anterior = ultimo;
      ultimo->proximo = novo_node;
      fila->Ultimo = novo_node; // Atualiza o Ultimo
    }
  }
  else{
    // Procura a posição correta para inserir o novo paciente
    Node *atual = fila->Primeiro;
    while (atual != NULL && novo_paciente->patologia[1] <= ((Paciente *)atual->dados)->patologia[1]) {
      atual = atual->proximo;
    }
    // Insere o novo_node na posição correta
    if (atual != NULL) {
      Node *anterior = atual->anterior;
      novo_node->proximo = atual;
      novo_node->anterior = anterior;
      if (anterior != NULL) {
        anterior->proximo = novo_node;
      } 
      else {
        // Se anterior é nulo, então o novo_node se torna o novo Primeiro
        fila->Primeiro = novo_node;
      }
      atual->anterior = novo_node;
    } 
    else {
      // Se chegou ao final da fila, insere no final
      Node *ultimo = fila->Ultimo;
      novo_node->anterior = ultimo;
      ultimo->proximo = novo_node;
      fila->Ultimo = novo_node; // Atualiza o Ultimo
    }

  }
  fila->tamanho++;
}

//foi feita para tirar os pacientes que esperam mais de 7200 U.T na fila
static void _tira_pos_tempo_da_fila(Node *no){
  Node *auxiliar = no;
  no->anterior->proximo = no->proximo;
  no->proximo->anterior = no->anterior;
  free(auxiliar);
}

void tira_pos_tempo(Lista *lista_controle, Fila *fila_SE, 
Fila *fila_raiox, Fila *fila_laudo, int Unidade_de_tempo){
  Node *SE = fila_SE->Ultimo;
  Node *raiox = fila_raiox->Ultimo;
  Node *laudo = fila_laudo->Ultimo;
  if(SE == NULL || raiox == NULL || laudo == NULL){
    return;
  }

  Paciente *p_SE;
  Paciente *p_raiox;
  Paciente *p_laudo;

  while(SE!=NULL && raiox!=NULL && laudo!=NULL){
    p_SE = SE->dados;
    if(Unidade_de_tempo - p_SE->entrada >=MAX_QUANTIDADE_UNIDADE_DE_TEMPO_ESPERA 
    && (p_SE->entrada_raiox == 0 || p_SE->entrada_laudo == 0)){
      for(Node *controle = lista_controle->Primeiro; controle != NULL; controle = controle->proximo){
        if(((Paciente*)controle->dados)->id == p_SE->id){
          controle->dados = p_SE;
          _tira_pos_tempo_da_fila(SE);

        }
      }
    }
    SE = SE->anterior;
    p_raiox = raiox->dados;
    if(Unidade_de_tempo - p_raiox->entrada >=MAX_QUANTIDADE_UNIDADE_DE_TEMPO_ESPERA 
    && (p_raiox->entrada_raiox == 0 || p_raiox->entrada_laudo == 0)){
      for(Node *controle = lista_controle->Primeiro; controle != NULL; controle = controle->proximo){
        if(((Paciente*)controle->dados)->id == p_raiox->id){
          controle->dados = p_raiox;
          _tira_pos_tempo_da_fila(raiox);
        }
      }
    }
    raiox = raiox->anterior;
    p_laudo = laudo->dados;
    if(Unidade_de_tempo - p_laudo->entrada >=MAX_QUANTIDADE_UNIDADE_DE_TEMPO_ESPERA 
    && (p_laudo->entrada_raiox == 0 || p_laudo->entrada_laudo == 0)){
      for(Node *controle = lista_controle->Primeiro; controle != NULL; controle = controle->proximo){
        if(((Paciente*)controle->dados)->id == p_laudo->id){
          controle->dados = p_laudo;
          _tira_pos_tempo_da_fila(laudo);
        }
      }
    }
    laudo = laudo->anterior;
  }
}

/*-----------------------------------------------------------------
                             TAD Log
-----------------------------------------------------------------*/
Log *cria_log(){
  Log *logging = (Log*)malloc(sizeof(Log));
  logging->count = 0;
  return logging;
}

void free_Log(Log *logging){
  free(logging);
}

void logging_evento(Log *logging, char *mensagem){
  int k = 0;
  for(int i = 0; mensagem[k]; i++) { 
    for(int j = 0; j<256; j++){  
      if((char)mensagem[k] == ';'){
        k++;
        break;
      }
      logging->eventos[i].message[j] = mensagem[k];
      k++;
    }
    logging->count++;
  }
  free(mensagem);
}

void save_log_to_file(const Log *logging, const char *filename){
  FILE *arquivo = fopen(filename, "w");
  if (arquivo == NULL) {
    printf("Erro ao abrir o arquivo.\n");
    exit(1);
  }
  for(int i = 0; i<logging->count; i++){
    fprintf(arquivo, "%s\n", logging->eventos[i].message);
  }
  fclose(arquivo);
}

char *mensagem(Lista *lista_controle) {
  char *msg;
  Node *atual = lista_controle->Primeiro;
  Paciente *paciente;
  char entrada_str[7], entrada_raiox_str[7], saida_raiox_str[7],  
  entrada_laudo_str[7], saida_str[7], idade[4];
  
  // Aloca a string de mensagem 
  msg = (char*) malloc((256) * sizeof(char) * MAX_QUANTIDADE_EVENTOS); 
  if (msg == NULL) { 
    printf("Erro ao alocar memória.\n"); 
    exit(1); 
  }

  // Preenche a string de mensagem
  atual = lista_controle->Ultimo;
  char patologia[20];
  while (atual != NULL) {
    paciente = atual->dados;

    //inicializa variáveis
    sprintf(entrada_str, "%d", paciente->entrada);
    sprintf(entrada_raiox_str, "%d", paciente->entrada_raiox);
    sprintf(saida_raiox_str, "%d", paciente->saida_raiox);
    sprintf(entrada_laudo_str, "%d", paciente->entrada_laudo);
    sprintf(saida_str, "%d", paciente->saida);
    sprintf(idade, "%d", paciente->idade);
    switch(paciente->patologia[0]){
      case 1:
        strcpy(patologia, "Saúde normal");
        break;
      case 2:
        strcpy(patologia, "Bronquite");
        break;
      case 3:
        strcpy(patologia, "Pneumonia");
        break;
      case 4:
        strcpy(patologia, "Fratura de fêmur");
        break;
      case 5:
        strcpy(patologia, "Apendicite");
        break;
    }

    //coloca o conteúdo na mensagem

    //informações do paciente
    strcat(msg, "Nome: ");
    strcat(msg, paciente->nome);
    strcat(msg, "\tCPF: ");
    strcat(msg, paciente->cpf);
    strcat(msg, "\tIdade: ");
    strcat(msg, idade);
    strcat(msg, "\tPatologia: ");
    strcat(msg, patologia);

    //informações de entrada e saida dentro do hospital
    strcat(msg, "\nEntrada: ");
    strcat(msg, entrada_str);
    strcat(msg, "\nEntrada no raio-x: ");
    strcat(msg, entrada_raiox_str);
    strcat(msg, "\nSaida do raio-x: ");
    strcat(msg, saida_raiox_str);
    strcat(msg, "\nEntrada na sala de laudo: ");
    strcat(msg, entrada_laudo_str);
    strcat(msg, "\nSaída: ");
    strcat(msg, saida_str);
    strcat(msg, "\n;");

    atual = atual->anterior;
  }
  return msg;
}
