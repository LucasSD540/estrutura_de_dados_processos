#include <stdio.h>
#include <stdlib.h>
#include "processo.h"

int main()
{
  int totalProcessos;
  Processo *processos = lerProcessos("processo_043_202409032338.csv", &totalProcessos);

  if (!processos)
  {
    printf("Erro ao ler processos.\n");
    return 1;
  }

  printf("Total de processos lidos: %d\n", totalProcessos);

  // 1. Ordenar por ID e salvar
  ordenarPorId(processos, totalProcessos);
  salvarProcessosCSV("processos_ordenados_id.csv", processos, totalProcessos);
  printf("Processos ordenados por ID salvos em 'processos_ordenados_id.csv'\n");

  // 2. Ordenar por data (decrescente) e salvar
  ordenarPorDataDecrescente(processos, totalProcessos);
  salvarProcessosCSV("processos_ordenados_data.csv", processos, totalProcessos);
  printf("Processos ordenados por data salvos em 'processos_ordenados_data.csv'\n");

  // 3. Contar processos por classe (exemplo: classe 12554)
  int id_classe = 12554;
  int count = contarProcessosPorClasse(processos, totalProcessos, id_classe);
  printf("Total de processos na classe %d: %d\n", id_classe, count);

  // 4. Contar assuntos diferentes
  int assuntosDiferentes = contarAssuntosDiferentes(processos, totalProcessos);
  printf("Total de assuntos diferentes: %d\n", assuntosDiferentes);

  printf("\nPressione ENTER para listar processos com múltiplos assuntos...");
  getchar();

  // 5. Listar processos com múltiplos assuntos
  listarProcessosComMultiplosAssuntos(processos, totalProcessos);

  // 6. Calcular dias de tramitação
  calcularDiasTramitacao(processos, totalProcessos);

  free(processos);
  return 0;
}