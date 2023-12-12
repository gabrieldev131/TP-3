# TP-3
Trabalho prático 3 - estrutura de dados

Neste trabalho, onde eu (Gabriel Dias Brito) e minha colega (Wal Candeia), produzimos um algoritmo em C ansi do qual tem o objetivo de organizar uma fila de pacientes em um hospital, do qual atende 5 tipos de patologias (Saúde normal, Bronquite, Pneumonia, Fratura de Fêmur e Apendicite), em que no hospital irão para uma sala de raio-x (das quais o hospital possui 5 salas) e logo após o raio-x irão tirar o laudo (dos quais o hospital possui 3 salas). Ao fim, deve-se registrar todos os acontecimentos em um arquivo chamado log.txt.

Pensando na organização, foi-se usado uma estrutura de dados do tipo Lista para armazenar os dados gerais dos pacientes com os horários de entrada e saida do hospital, assim como de cada sala, usando esses dados para o computo das métricas (médias para avaliação de desempenho do hospital, avaliando gargalos) E do tipo Fila, dos quais tem os pacientes e sua triagem no hospital.

cada paciente recebe nome, cpf, idade, id, patologia (identificador da doença e a gravidade dela), e os horários dos acontecimentos no hospital, assim como o hospital tem suas salas (raio-x e laudo), das quais possuem identificadores de se está vazia,  tempo da consulta e o id do paciente. dessa forma, é possivel organizar a fila de atendimento da qual é usada e atualizada para receber os pacientes mais prioritários (por ordem de gravidade da patologia).

A cada iteração do código, é chamado de unidade de tempo, da qual é usada para estabelecer quanto tempo irá durar a simulação, assim como o id do paciente. Pensando em analizar por esse modelo, foi pensado em a cada 10 unidade de tempo, seja printado na tela as métricas (tempo médio para se tirar um laudo, média por patologia e quantidade de exames realizados após o limite de tempo), com um intervalo dado pelo usuário no inicio do programa (em micro-segundos) para melhor visualização do que está ocorrendo nas métricas. 

para executar o programa (no linux ubuntu 22.04 com GCC 11), digite make na linha de comando, seguida por ./main, dessa forma o programa será executado

