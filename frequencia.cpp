/*
  Problema de Alocação de Frequências

  - Quando o raio de duas antenas se intercepta, não podem usar a mesma frequência para evitar interferência.
  - Objetivo: Atribuir uma frequência para cada antena, minimizando o número total de frequências utilizadas
    e garantindo que antenas vizinhas não compartilhem a mesma frequência.
*/

#include <ilcplex/ilocplex.h>
#include <vector>
#include <iostream>

ILOSTLBEGIN;

int main() {
    IloEnv env;
    IloModel model(env);

    const int n = 9; // Número de antenas

    // Lista de adjacência: pares de antenas que interferem
    std::vector<std::vector<int>> g(n);

    g[0] = {1, 2, 3, 7};
    g[1] = {0, 3};
    g[2] = {0, 3};
    g[3] = {0, 1, 2, 4, 5, 6};
    g[4] = {3};
    g[5] = {3, 8};
    g[6] = {3, 7};
    g[7] = {0, 6};
    g[8] = {5};

    // x[i][k] = 1 se antena i usa frequência k
    IloArray<IloIntVarArray> x(env, n);
    for (int i = 0; i < n; ++i) {
        x[i] = IloIntVarArray(env, n, 0, 1); // No máximo n frequências
    }

    // z[k] = 1 se a frequência k é usada por pelo menos uma antena
    IloIntVarArray z(env, n, 0, 1);

    // Função Objetivo: Minimizar o número de frequências usadas
    IloExpr obj(env);
    for (int k = 0; k < n; ++k)
        obj += z[k];
    model.add(IloMinimize(env, obj));

    // Cada antena deve usar exatamente uma frequência
    for (int i = 0; i < n; ++i) {
        IloExpr soma_freq(env);
        for (int k = 0; k < n; ++k)
            soma_freq += x[i][k];
        model.add(soma_freq == 1);
    }

    // Frequência k só pode ser usada por antena i se k for realmente ativada (z[k] == 1)
    for (int i = 0; i < n; ++i) {
        for (int k = 0; k < n; ++k)
            model.add(x[i][k] <= z[k]);
    }

    // Antenas vizinhas não podem usar a mesma frequência
    for (int i = 0; i < n; ++i) {
        for (int j : g[i]) {
            if (i < j) { // evita duplicar restrições (grafo não-direcionado)
                for (int k = 0; k < n; ++k)
                    model.add(x[i][k] + x[j][k] <= 1);
            }
        }
    }

    // Resolver o modelo
    IloCplex cplex(model);
    if (!cplex.solve()) {
        std::cout << "Problema não resolvido.\n";
        return 1;
    }

    std::cout << "Problema resolvido com sucesso!\n";
    std::cout << "Número mínimo de frequências utilizadas: " << cplex.getObjValue() << "\n\n";

    // Mostrar frequência atribuída para cada antena
    for (int i = 0; i < n; ++i) {
        for (int k = 0; k < n; ++k) {
            if (cplex.getValue(x[i][k]) > 0.5) {
                std::cout << "Antena " << i << " usa frequência " << k << "\n";
                break;
            }
        }
    }

    env.end();
    return 0;
}
