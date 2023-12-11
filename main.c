#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "exames.h"

int main(){
  Lista *lista_controle = inicia_Lista();//lista não ordenada
  Fila *fila_SE = inicia_Fila();//fila da sala de espera (já ordenada)
  Fila *fila_raiox = inicia_Fila();//fila de quem vai fazer/está fazendo raiox (já ordenado)
  Fila *fila_laudo = inicia_Fila();//fila de quem vai fazer/está fazendo raiox (já ordenado)
  Hospital *hospital = inicia_Hospital();//estrutura que controla os equipamentos e funcionários do hospital
  Log *logging = cria_log();//estrutura que guarda as informações dos pacientes para coloca-los em um arquivo
  srand(time(NULL));
  
  for(int Unidade_de_tempo = 1;Unidade_de_tempo<43201;Unidade_de_tempo++){
    
    //gera o paciente
    gera_paciente(lista_controle, fila_SE, Unidade_de_tempo);

    //sequência de verificações para olhar o tempo da consulta e as máquinas desocupadas e inserir o paciente nas filas
    atualiza_hospital(hospital, fila_SE, fila_raiox,fila_laudo, Unidade_de_tempo);

    //se o paciente esperar muito, ele sai
    tira_pos_tempo(lista_controle, fila_SE, fila_raiox, fila_laudo, Unidade_de_tempo);

    //atualiza a lista que controla em que momento o paciente entrou ou saiu de alguma ala do hospital
    atualiza_lista_controle(fila_laudo, lista_controle);

    //a cada 10 unidades de tempo, ela entra na condição
    if((Unidade_de_tempo%10) == 0){
      //função que printa as médias de tempo
      printa_metrica(lista_controle, hospital);
      //faz esperar 0,5 segundos
      usleep(500000);
    }
  }

  //coloca a mensagem na estrutura
  logging_evento(logging, mensagem(lista_controle));

  //coloca o conteudo da estrutura no arquivo
  save_log_to_file(logging, "log.txt");

  //liberando memória
  liberaLista(lista_controle);
  liberaFila(fila_raiox);
  liberaFila(fila_laudo);
  liberaFila(fila_SE);
  liberaHospital(hospital);
  return 0;
}
