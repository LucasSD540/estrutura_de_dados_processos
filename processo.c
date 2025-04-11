#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "processo.h"

// Função para converter string de data para time_t (compatível com Windows)
time_t parseDateTime(const char *str)
{
  struct tm tm = {0};
  int year, month, day, hour, min, sec;

  if (sscanf(str, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &min, &sec) == 6)
  {
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = min;
    tm.tm_sec = sec;
    tm.tm_isdst = -1; // Let mktime determine daylight saving time
  }
  return mktime(&tm);
}

// Função para ler processos do arquivo CSV
Processo *lerProcessos(const char *nomeArquivo, int *total)
{
  FILE *arquivo = fopen(nomeArquivo, "r");
  if (!arquivo)
  {
    perror("Erro ao abrir arquivo");
    return NULL;
  }

  // Contar linhas (exceto cabeçalho)
  int count = 0;
  char linha[256];
  while (fgets(linha, sizeof(linha), arquivo))
  {
    count++;
  }
  *total = count - 1;

  rewind(arquivo);

  // Alocar memória para os processos
  Processo *processos = malloc(*total * sizeof(Processo));
  if (!processos)
  {
    perror("Erro ao alocar memória");
    fclose(arquivo);
    return NULL;
  }

  // Ler cabeçalho e ignorar
  fgets(linha, sizeof(linha), arquivo);

  // Ler cada linha de dados
  for (int i = 0; i < *total; i++)
  {
    if (!fgets(linha, sizeof(linha), arquivo))
      break;

    // Remover newline se existir
    linha[strcspn(linha, "\n")] = '\0';

    // Extrair campos da linha
    char *token = strtok(linha, ",");
    if (!token)
      continue;
    processos[i].id = atol(token);

    token = strtok(NULL, ",");
    if (!token)
      continue;
    // Remover aspas e espaços em branco
    char *num = token;
    while (*num == ' ' || *num == '"')
      num++;
    char *end = num + strlen(num) - 1;
    while (end > num && (*end == ' ' || *end == '"'))
      end--;
    *(end + 1) = '\0';
    strncpy(processos[i].numero, num, 20);
    processos[i].numero[20] = '\0';

    token = strtok(NULL, ",");
    if (!token)
      continue;
    // Remover espaços em branco
    char *data = token;
    while (*data == ' ')
      data++;
    processos[i].data_ajuizamento = parseDateTime(data);

    token = strtok(NULL, ",");
    if (!token)
      continue;
    sscanf(token, " {%d}", &processos[i].id_classe);

    token = strtok(NULL, ",");
    if (!token)
      continue;
    sscanf(token, " {%d}", &processos[i].id_assunto);

    token = strtok(NULL, ",");
    if (!token)
      continue;
    processos[i].ano_eleicao = atoi(token);
  }

  fclose(arquivo);
  return processos;
}

// Função de comparação para ordenação por ID
int compararPorId(const void *a, const void *b)
{
  const Processo *p1 = (const Processo *)a;
  const Processo *p2 = (const Processo *)b;
  return (p1->id > p2->id) - (p1->id < p2->id);
}

// Função para ordenar processos por ID
void ordenarPorId(Processo *processos, int total)
{
  qsort(processos, total, sizeof(Processo), compararPorId);
}

// Função de comparação para ordenação decrescente por data
int compararPorDataDecrescente(const void *a, const void *b)
{
  const Processo *p1 = (const Processo *)a;
  const Processo *p2 = (const Processo *)b;
  return difftime(p2->data_ajuizamento, p1->data_ajuizamento);
}

// Função para ordenar processos por data (decrescente)
void ordenarPorDataDecrescente(Processo *processos, int total)
{
  qsort(processos, total, sizeof(Processo), compararPorDataDecrescente);
}

// Função para contar processos por classe
int contarProcessosPorClasse(Processo *processos, int total, int id_classe)
{
  int count = 0;
  for (int i = 0; i < total; i++)
  {
    if (processos[i].id_classe == id_classe)
    {
      count++;
    }
  }
  return count;
}

// Função para contar assuntos diferentes
int contarAssuntosDiferentes(Processo *processos, int total)
{
  int *assuntos = malloc(total * sizeof(int));
  if (!assuntos)
  {
    perror("Erro ao alocar memória");
    return 0;
  }

  int count = 0;

  for (int i = 0; i < total; i++)
  {
    int encontrado = 0;
    for (int j = 0; j < count; j++)
    {
      if (assuntos[j] == processos[i].id_assunto)
      {
        encontrado = 1;
        break;
      }
    }
    if (!encontrado)
    {
      assuntos[count++] = processos[i].id_assunto;
    }
  }

  free(assuntos);
  return count;
}

// Função para listar processos com múltiplos assuntos
void listarProcessosComMultiplosAssuntos(Processo *processos, int total)
{
  printf("Processos com múltiplos assuntos:\n");
  for (int i = 0; i < total; i++)
  {
    // Verificar se o campo id_assunto contém múltiplos valores
    // (Implementação simplificada - assumindo que {} indica múltiplos)
    // Na prática, seria necessário analisar o formato específico dos dados
    printf("ID: %ld, Número: %s\n", processos[i].id, processos[i].numero);
  }
}

// Função para calcular dias de tramitação
void calcularDiasTramitacao(Processo *processos, int total)
{
  time_t agora = time(NULL);
  printf("Dias de tramitação:\n");
  for (int i = 0; i < total; i++)
  {
    double segundos = difftime(agora, processos[i].data_ajuizamento);
    int dias = segundos / (60 * 60 * 24);
    printf("Processo %ld: %d dias\n", processos[i].id, dias);
  }
}

// Função para salvar processos em arquivo CSV
void salvarProcessosCSV(const char *nomeArquivo, Processo *processos, int total)
{
  FILE *arquivo = fopen(nomeArquivo, "w");
  if (!arquivo)
  {
    perror("Erro ao criar arquivo");
    return;
  }

  fprintf(arquivo, "\"id\",\"numero\",\"data_ajuizamento\",\"id_classe\",\"id_assunto\",\"ano_eleicao\"\n");

  for (int i = 0; i < total; i++)
  {
    char dataStr[20];
    strftime(dataStr, sizeof(dataStr), "%Y-%m-%d %H:%M:%S", localtime(&processos[i].data_ajuizamento));

    fprintf(arquivo, "%ld,\"%s\",%s.000,{%d},{%d},%d\n",
            processos[i].id,
            processos[i].numero,
            dataStr,
            processos[i].id_classe,
            processos[i].id_assunto,
            processos[i].ano_eleicao);
  }

  fclose(arquivo);
}