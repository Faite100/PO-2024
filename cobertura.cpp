/*
  O governo precisa decidir onde construir escolas em uma cidade.
  Cada bairro deve ter uma escola OU estar conectado a um bairro com escola.
  O objetivo é minimizar o número total de escolas construídas.
*/

#include <ilcplex/ilocplex.h>
#include <vector>
#include <iostream>

ILOSTLBEGIN

// Lista de conexões entre bairros (arestas do grafo)
const std::vector<std::pair<int, int>> EDGES = {
  {0, 1}, {0, 2}, {0, 3}, {0, 7}, {1, 3}, {2, 3},
  {3, 4}, {3, 5}, {3, 6}, {5, 8}, {6, 7},
};

int main() {
  IloEnv env;
  IloModel model(env);

  const int n = 9; // número de bairros

  // Matriz de adjacência para representar o grafo
  std::vector<std::vector<int>> adj(n, std::vector<int>(n, 0));
  for (const auto& [u, v] : EDGES) {
    adj[u][v] = 1;
    adj[v][u] = 1;
  }

  // Variável binária: x[i] = 1 se há uma escola no bairro i
  IloBoolVarArray x(env, n);

  // Função objetivo: minimizar o número total de escolas
  IloExpr total_schools(env);
  for (int i = 0; i < n; ++i) {
    total_schools += x[i];
  }
  model.add(IloMinimize(env, total_schools));

  // Restrição: cada bairro deve ter uma escola ou estar conectado a um que tenha
  for (int i = 0; i < n; ++i) {
    IloExpr coverage(env);
    coverage += x[i]; // escola no próprio bairro
    for (int j = 0; j < n; ++j) {
      if (adj[i][j] == 1) {
        coverage += x[j]; // escola nos vizinhos
      }
    }
    model.add(coverage >= 1); // garantir cobertura
  }

  IloCplex cplex(model);

  if (cplex.solve()) {
    std::cout << "Solução encontrada!\n";
    std::cout << "Número mínimo de escolas: " << cplex.getObjValue() << "\n";
    for (int i = 0; i < n; ++i) {
      if (cplex.getValue(x[i]) > 0.5) {
        std::cout << "Escola no bairro " << i << "\n";
      }
    }
  } else {
    std::cout << "Não foi possível encontrar solução.\n";
  }

  return 0;
}
