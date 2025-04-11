#ifndef PROCESSO_H
#define PROCESSO_H

#include <time.h>

typedef struct
{
  long id;
  char numero[21];
  time_t data_ajuizamento;
  int id_classe;
  int id_assunto;
  int ano_eleicao;
} Processo;

// Funções para manipulação de processos
Processo *lerProcessos(const char *nomeArquivo, int *total);
void ordenarPorId(Processo *processos, int total);
void ordenarPorDataDecrescente(Processo *processos, int total);
int contarProcessosPorClasse(Processo *processos, int total, int id_classe);
int contarAssuntosDiferentes(Processo *processos, int total);
void listarProcessosComMultiplosAssuntos(Processo *processos, int total);
void calcularDiasTramitacao(Processo *processos, int total);
void salvarProcessosCSV(const char *nomeArquivo, Processo *processos, int total);

#endif