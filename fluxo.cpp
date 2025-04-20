/*
  Problema de Fluxo Máximo

  Um produtor de gás natural (s) precisa enviar a maior quantidade possível de gás para uma fábrica (t).
  Cada duto tem uma direção fixa e uma capacidade máxima de transporte.

  Objetivo: Maximizar o fluxo total que sai da origem s e chega ao destino d.
*/

#include <ilcplex/ilocplex.h>
#include <vector>
#include <iostream>

ILOSTLBEGIN;

// Representação do grafo: matriz de adjacência com capacidades
const std::vector<std::vector<int>> GRAFO = {
  {0, 10, 5, 0},   // s → {1,2}
  {0, 0, 15, 8},   // 1 → {2,3}
  {0, 0, 0, 10},   // 2 → 3
  {0, 0, 0, 0}     // d
};

int main() {
  IloEnv env;
  IloModel model(env);

  int n = GRAFO.size();
  int s = 0; // origem (source)
  int d = 3; // destino (sink)

  // Variáveis de decisão: x[i][j] representa o fluxo do nó i para o nó j
  IloArray<IloNumVarArray> x(env, n);
  for (int i = 0; i < n; ++i) {
    x[i] = IloNumVarArray(env, n);
    for (int j = 0; j < n; ++j) {
      // Fluxo máximo limitado pela capacidade do duto
      x[i][j] = IloIntVar(env, 0, GRAFO[i][j]);
    }
  }

  // Função objetivo: maximizar o fluxo total saindo da origem (s)
  IloExpr fluxo_saida(env);
  for (int j = 0; j < n; ++j)
    fluxo_saida += x[s][j];

  model.add(IloMaximize(env, fluxo_saida));

  // Restrição de conservação de fluxo para nós intermediários (exceto s e d)
  for (int i = 0; i < n; ++i) {
    if (i == s || i == d) continue;

    IloExpr entrada(env);
    IloExpr saida(env);

    for (int j = 0; j < n; ++j) {
      entrada += x[j][i]; // fluxo entrando no nó i
      saida += x[i][j];   // fluxo saindo do nó i
    }

    model.add(entrada == saida); // fluxo que entra deve ser igual ao que sai
  }

  // Resolver o modelo
  IloCplex cplex(model);
  if (!cplex.solve()) {
    std::cout << "Problema não resolvido.\n";
    return 1;
  }

  std::cout << "Problema resolvido!\n";
  std::cout << "Fluxo máximo: " << cplex.getObjValue() << "\n\n";

  // Exibir valores de fluxo utilizados nas arestas
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      double val = cplex.getValue(x[i][j]);
      if (val > 0)
        std::cout << "Fluxo de " << i << " → " << j << ": " << val << "\n";
    }
  }

  env.end();
  return 0;
}
