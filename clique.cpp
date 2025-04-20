/*
  Dado um grafo G = (V, E), determinar a clique máxima,
  ou seja, o maior subconjunto de vértices onde todos estão conectados entre si.
*/

#include <ilcplex/ilocplex.h>
#include <vector>

ILOSTLBEGIN

int main() {
    IloEnv env;
    IloModel model(env);

    const int n = 10; // número de vértices

    // Variáveis binárias: x[i] = 1 se o vértice i está na clique
    IloIntVarArray x(env, n, 0, 1);

    // Lista de adjacência do grafo
    std::vector<std::vector<int>> g(n);
    std::vector<std::pair<int, int>> edges = {
        {0,1}, {0,2}, {0,3}, {1,3}, {2,5}, {2,3}, {5,6}, {5,7},
        {6,3}, {6,9}, {1,4}, {3,4}, {9,7}, {8,9}, {6,7}, {3,6}
    };

    for (auto [u, v] : edges) {
        g[u].push_back(v);
        g[v].push_back(u);
    }

    // Função objetivo: maximizar a quantidade de vértices na clique
    IloExpr clique_size(env);
    for (int v = 0; v < n; ++v)
        clique_size += x[v];

    model.add(IloMaximize(env, clique_size));

    // Adiciona restrições para garantir que apenas vértices adjacentes
    // possam estar juntos na clique
    for (int v = 0; v < n; ++v) {
        for (int u = v + 1; u < n; ++u) {
            // Verifica se não há aresta entre v e u
            bool are_connected = false;
            for (int w : g[v]) {
                if (w == u) {
                    are_connected = true;
                    break;
                }
            }

            if (!are_connected) {
                model.add(x[v] + x[u] <= 1);
            }
        }
    }

    IloCplex cplex(model);

    if (cplex.solve()) {
        std::cout << "Solução encontrada!\n";
        std::cout << "Tamanho da clique: " << cplex.getObjValue() << "\n";
        std::cout << "Vértices na clique: ";
        for (int v = 0; v < n; ++v) {
            if (cplex.getValue(x[v]) > 0.5) {
                std::cout << v << " ";
            }
        }
        std::cout << "\n";
    } else {
        std::cout << "Não foi possível resolver o problema.\n";
    }

    return 0;
}
