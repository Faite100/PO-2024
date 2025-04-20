/*
  – Os depósitos possuem demandas que precisam ser atendidas.
  – As fábricas possuem ofertas para atender os depósitos
  – Cada arco possui um custo de transporte cij entre a fábrica i e o depósito j .
  – Queremos atender as demandas dos depósitos com um custo mínimo de
  transporte, respeitando as restrições.

  Fab-Dep | Depósito 1 | Depósito 2 | Depósito 3
  Fáb 1   | 8          | 5          | 6
  Fáb 2   | 15         | 10         | 12
  Fáb 3   | 3          | 9          | 10
  
  Oferta das Fábricas:
  - Fábrica 1: 120 unidades
  - Fábrica 2: 80 unidades
  - Fábrica 3: 80 unidades

  Demanda dos Depósitos:
  - Depósito 1: 150 unidades
  - Depósito 2: 70 unidades
  - Depósito 3: 60 unidades
*/

#include <ilcplex/ilocplex.h>
#include <vector>

ILOSTLBEGIN;

const std::vector<std::vector<int>> CUSTOS = {
  { 8, 5, 6 },  // Custos de transporte da Fábrica 1 para os depósitos 1, 2, 3
  { 15, 10, 12 },  // Custos de transporte da Fábrica 2 para os depósitos 1, 2, 3
  { 3, 9, 10 },  // Custos de transporte da Fábrica 3 para os depósitos 1, 2, 3
};

const std::vector<int> CAPACIDADES = {
  120, 80, 80  // Capacidades das fábricas 1, 2, 3
};

const std::vector<int> DEMANDAS = {
  150, 70, 60  // Demandas dos depósitos 1, 2, 3
};

int main() {
  IloEnv env;
  IloModel model(env);

  // Definindo variáveis de decisão x[i][j], onde:
  // x[i][j] é a quantidade transportada da fábrica i para o depósito j
  IloArray<IloIntVarArray> x(env, 3);  // 3 fábricas e 3 depósitos

  // Inicializando as variáveis de decisão, com limites de 0 até infinito
  for (unsigned i = 0; i < 3; i++) {
    x[i] = IloIntVarArray(env, 3, 0, IloInfinity);  // Cada fábrica tem 3 depósitos
  }

  // Função Objetivo: Minimizar o custo de transporte
  IloExpr obj_expr(env);
  
  // Percorre todos os pares de fábricas e depósitos para calcular o custo total
  for (unsigned i = 0; i < 3; i++) {
    for (unsigned j = 0; j < 3; j++) {
      obj_expr += CUSTOS[i][j] * x[i][j];  // Custo de transporte entre fábrica i e depósito j
    }
  }

  // Definindo a função objetivo como minimização do custo
  IloObjective obj = IloMinimize(env, obj_expr);
  model.add(obj);  // Adiciona a função objetivo ao modelo

  // Restrições de capacidade das fábricas
  // A quantidade de unidades enviadas de cada fábrica não pode exceder sua capacidade
  for (unsigned i = 0; i < 3; i++) {
    model.add((x[i][0] + x[i][1] + x[i][2]) <= CAPACIDADES[i]);  // Soma das variáveis para a fábrica i
  }

  // Restrições de demanda dos depósitos
  // A quantidade total recebida por cada depósito deve ser igual à sua demanda
  for (unsigned i = 0; i < 3; i++) {
    model.add((x[0][i] + x[1][i] + x[2][i]) == DEMANDAS[i]);  // Soma das variáveis para o depósito i
  }

  // Resolver o modelo usando CPLEX
  IloCplex cplex(model);
  IloBool result = cplex.solve();  // Tenta resolver o problema de otimização

  // Verifica se a solução foi encontrada
  if (result) {
    printf("Problema resolvido!\n");
    printf("Custo mínimo: %f\n", cplex.getObjValue());  // Imprime o custo mínimo encontrado
  } else {
    printf("Problema não resolvido\n");
  }

  return 0;
}
